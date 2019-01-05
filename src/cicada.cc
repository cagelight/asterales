#include "asterales/cicada.hh"

#include <algorithm>

#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/sendfile.h>
#include <netdb.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/epoll.h>

using namespace asterales::cicada;

static constexpr int enable = 1;
static constexpr int disable = 0;

#define TMPBUF_SIZE 512

#define EPOLLEVT reinterpret_cast<epoll_event *>(epoll_evt)

#define MAX_EPOLL_EVENTS 128
#define EPOLLMEVT reinterpret_cast<epoll_event *>(epoll_mevt)

socket::~socket() {
	if (FD != -1) {
		shutdown(FD, SHUT_RDWR);
		::close(FD);
	}
}

void socket::close() {
	if (FD != -1) {
		shutdown(FD, SHUT_RDWR);
		::close(FD);
	}
	FD = -1;
}

struct sendfile_helper::impl_t {
	int fd;
	struct stat finfo;
	off_t offset;
	size_t count;
	impl_t(std::string const & path, off_t offset, size_t count) : offset(offset), count(count) {
		fd = ::open(path.c_str(), O_RDONLY);
		if (fd < 0) throw exception::sendfile_notfound {};
		fstat(fd, &finfo);
		if (!S_ISREG(finfo.st_mode)) throw exception::sendfile_notfile {};
		if (offset > finfo.st_size) throw exception::sendfile_badoffset {};
		if (offset + count > static_cast<size_t>(finfo.st_size)) count = finfo.st_size - offset;
	}
};

sendfile_helper::sendfile_helper(std::string const & path, off_t offset, size_t count) {
	impl.reset(new impl_t {path, offset, count});
}

bool sendfile_helper::is_done() {
	return !impl->count;
}

ssize_t sendfile_helper::work(connection & con) {
	ssize_t amt = con.sendfile(impl->fd, &impl->offset, impl->count);
	if (amt < 0) return -1;
	impl->count -= amt;
	return amt;
}

connection::connection(socket && sock) : socket(sock) {
	sock.FD = -1;
}

connection::connection(connection && sock) : socket(sock) {
	sock.FD = -1;
}

ssize_t connection::read(char * buf, size_t buf_len) {
	ssize_t e = recv(FD, buf, buf_len, 0);
	if (e == 0) return -1;
	else if (e < 0) {
		if (errno == EAGAIN
			#if EAGAIN != EWOULDBLOCK
			|| errno == EWOULDBLOCK
			#endif
		) return 0;
		else return -1;
	} else return e;
}

ssize_t connection::read(buffer_assembly & buf, size_t cnt) {
	char tmpbuf [TMPBUF_SIZE];
	ssize_t ret = 0;
	while (true) {
		size_t readnum = cnt < TMPBUF_SIZE ? cnt : TMPBUF_SIZE;
		ssize_t e = connection::read(tmpbuf, readnum);
		if (e < 0) return -1;
		if (e == 0) break;
		ret += e;
		cnt -= e;
		buf.write(&tmpbuf[0], e);
		if (static_cast<size_t>(e) < readnum) break;
	}
	return ret;
}

ssize_t connection::write(char const * buf, size_t buf_len) {
	ssize_t e = send(FD, buf, buf_len, 0);
	if (e < 0) {
		if (errno == EAGAIN
			#if EAGAIN != EWOULDBLOCK
			|| errno == EWOULDBLOCK
			#endif
		) return 0;
		else return -1;
	} else return e;
}

ssize_t connection::write(buffer_assembly const & buf, size_t cnt) {
	if (cnt > buf.size()) cnt = buf.size();
	if (!cnt) return 0;
	return connection::write(reinterpret_cast<char const *>(buf.data()), cnt);
}

ssize_t connection::write_consume(buffer_assembly & buf, size_t cnt) {
	if (cnt > buf.size()) cnt = buf.size();
	if (!cnt) return 0;
	ssize_t e = connection::write(reinterpret_cast<char const *>(buf.data()), cnt);
	if (e <= 0) return e;
	if (static_cast<size_t>(e) == buf.size()) buf.clear();
	else buf.discard(e);
	return e;
}

ssize_t connection::sendfile(int fd, off_t * offs, size_t size) {
	ssize_t e = ::sendfile(FD, fd, offs, size);
	if (e < 0) {
		if (errno == EAGAIN
			#if EAGAIN != EWOULDBLOCK
			|| errno == EWOULDBLOCK
			#endif
		) return 0;
		else return -1;
	} else return e;
}

ssize_t connection::sendfile(sendfile_helper & sh) {
	ssize_t e = sh.work(*this);
	if (e < 0) {
		if (errno == EAGAIN
			#if EAGAIN != EWOULDBLOCK
			|| errno == EWOULDBLOCK
			#endif
		) return 0;
		else return -1;
	} else return e;
}

listener::listener(uint16_t port, accept_cb && cb) : _accept_cb(std::forward<accept_cb &&>(cb)) {
	FD = ::socket(PF_INET6, SOCK_STREAM | SOCK_NONBLOCK, 0);
	if (FD == -1) throw exception::socket_acquire {};
	setsockopt(FD, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable));
	setsockopt(FD, SOL_SOCKET, SO_REUSEPORT, &enable, sizeof(enable));
	setsockopt(FD, IPPROTO_IPV6, IPV6_V6ONLY, &disable, sizeof(disable));
	
	addr.sin6_family = AF_INET6;
	addr.sin6_addr = in6addr_any;
	addr.sin6_port = htons(port);
	
	if (bind(FD, reinterpret_cast<struct sockaddr *>(&addr), sizeof(addr)) < 0) throw exception::socket_bind {};
	if (listen(FD, SOMAXCONN)) throw exception::listen_start {};
}

void listener::accept() noexcept {
	while (true) {
		socket temp;
		socklen_t slen = sizeof(temp.addr);
		temp.FD = accept4(FD, reinterpret_cast<struct sockaddr *>(&temp.addr), &slen, SOCK_NONBLOCK);
		if (temp.FD == -1) return;
		_accept_cb(std::move(temp));
	}
}

reactor::reactor(bool create_master_thread, unsigned int workers_num) : last_pulse { asterales::time::now<asterales::time::clock_type::monotonic>() } {
	epoll_obj = epoll_create(1);
	epoll_mevt = new epoll_event [MAX_EPOLL_EVENTS];
	for (unsigned int i = 0; i < workers_num; i++) workers.push_back( new std::thread { &reactor::worker_run, this } );
	if (create_master_thread) master_thread = new std::thread { [this](){ while (run_sem) master_loop(); } };
}

reactor::~reactor() {
	run_sem.store(false);
	if (master_thread) {
		if (master_thread->joinable()) master_thread->join();
		delete master_thread;
	}
	m2w_cv_mut.lock();
	m2w_cv_mut.unlock();
	m2w_cv.notify_all();
	for (std::thread * worker : workers) {
		if (worker->joinable()) worker->join();
		delete worker;
	}
	close(epoll_obj);
	if (EPOLLMEVT) delete [] EPOLLMEVT;
}

void reactor::epoll_register(int fd) {
	epoll_event evt {};
	evt.data.fd = fd;
	evt.events = EPOLLIN;
	epoll_ctl(epoll_obj, EPOLL_CTL_ADD, fd, &evt);
}

void reactor::master(std::function<bool()> pred) {
	while (pred()) master_loop();
}

void reactor::master_loop() {
	
	int nfd = epoll_wait(epoll_obj, EPOLLMEVT, MAX_EPOLL_EVENTS, 1000);
	if (nfd < 0) {
		printf("ERROR: epoll_wait returned %i\n", nfd);
		run_sem.store(false);
		return;
	}
	
	service_lock.lock();
	for (auto & li : services) {
		li.second->accept();;
	}
	service_lock.unlock();
	
	m2w_lock.lock();
	for (int i = 0; i < nfd; i++) {
		reason::type rsn = 0;
		if (EPOLLMEVT[i].events & EPOLLIN) rsn |= reason::read_available;
		if (EPOLLMEVT[i].events & EPOLLOUT) rsn |= reason::write_available;
		m2w_queue.emplace(static_cast<int>(EPOLLMEVT[i].data.fd), rsn);
	}
	m2w_lock.unlock();
	
	auto now = asterales::time::now<asterales::time::clock_type::monotonic>();
	if (now - last_pulse > asterales::time::span {5}) {
		last_pulse = now;
		for (auto & i : instances) {
			m2w_lock.lock();
			m2w_queue.emplace(i.first, reason::pulse);
			m2w_lock.unlock();
		}
	}
	
	m2w_cv_mut.lock();
	m2w_cv_mut.unlock();
	m2w_cv.notify_all();
}
void reactor::worker_run() {
	
	while (run_sem) {
		{
			std::unique_lock<std::mutex> lk {m2w_cv_mut};
			if (!run_sem) return;
			m2w_cv.wait_for(lk, std::chrono::milliseconds(5000), [this] {
				std::lock_guard {m2w_lock};
				return (!run_sem) || (!m2w_queue.empty());
			});
		}
		
		if (!run_sem) return;
		
		while (true) {
			
			std::unique_lock<asterales::spinlock> m2w_ulk {m2w_lock};
			if (m2w_queue.empty()) break;
			auto msg = m2w_queue.front();
			m2w_queue.pop();
			m2w_ulk.unlock();
			
			instance_lock.read_access();
			auto inst_find = instances.find(msg.descriptor);
			if (inst_find == instances.end()) {
				instance_lock.read_done();
				continue;
			}
			
			std::shared_ptr<instance> inst = inst_find->second;
			instance_lock.read_done();
			
			if (!inst->use_lock.try_lock()) continue;
			detail d { msg.r };
			
			signal sig {};
			
			try {
				sig = inst->proto->ready(inst->con, d);
			} catch (exception::generic const & e) {
				printf("WARNING: a connection was terminated after catching a generic exception with the following message:\n%s\n", e.what());
				sig.m |= signal::mask::terminate;
			} catch (...) {
				printf("WARNING: a connection was terminated after catching an uncaught exception\n");
				sig.m |= signal::mask::terminate;
			}
			
			if (sig.m & signal::mask::terminate) {
				instance_lock.write_lock();
				instances.erase(msg.descriptor);
				instance_lock.write_unlock();
				inst->use_lock.unlock();
				continue;
			}
			
			if (sig.m & signal::mask::switch_protocols) {
				inst->proto = std::move(sig.protocol_switch);
				sig.m = inst->proto->default_mask();
			}
			
			int flags = 0;
			if (sig.m & signal::mask::wait_for_read) flags |= EPOLLIN;
			if (sig.m & signal::mask::wait_for_write) flags |= EPOLLOUT;
			inst->update_epoll(flags);
			
			inst->use_lock.unlock();
		}
	}
}

reactor::instance::instance(reactor const & parent, connection && con, std::unique_ptr<protocol> && pr) : parent(parent), con(std::forward<connection>(con)), proto(std::forward<std::unique_ptr<protocol> &&>(pr)) {
	proto->set_mask = [this](signal::mask::type new_mask){
		if (new_mask & signal::mask::terminate) {
			this->con.close();
			return;
		}
		int evt = 0;
		if (new_mask & signal::mask::wait_for_read) evt |= EPOLLIN;
		if (new_mask & signal::mask::wait_for_write) evt |= EPOLLOUT;
		this->update_epoll(evt);
	};
	
	epoll_evt = new epoll_event {};
	EPOLLEVT->data.fd = this->con.FD;
	EPOLLEVT->events = EPOLLONESHOT;
	auto dmask = proto->default_mask();
	if (dmask & signal::mask::wait_for_read) EPOLLEVT->events |= EPOLLIN;
	if (dmask & signal::mask::wait_for_write) EPOLLEVT->events |= EPOLLOUT;
	epoll_ctl(parent.epoll_obj, EPOLL_CTL_ADD, this->con.FD, EPOLLEVT);
}
reactor::instance::~instance() {
	epoll_ctl(parent.epoll_obj, EPOLL_CTL_DEL, con.FD, EPOLLEVT);
	if (EPOLLEVT) delete EPOLLEVT;
}

void reactor::instance::update_epoll(int flags) {
	flags |= EPOLLONESHOT;
	EPOLLEVT->events = flags;
	epoll_ctl(parent.epoll_obj, EPOLL_CTL_MOD, con.FD, EPOLLEVT);
}

struct connection_protocol : reactor::protocol {
	connection_protocol() = default;
	~connection_protocol() {
		if (addr) freeaddrinfo(addr);
	}
	
	virtual reactor::signal ready(connection & c, reactor::detail const &) override {
		reactor::signal s;
		
		int serr;
		socklen_t len;
		int gerr = getsockopt(c.FD, SOL_SOCKET, SO_ERROR, &serr, &len);
		
		if (gerr == -1) throw exception::connection_establish {};
		if (serr == EINPROGRESS) {
			s.m = reactor::signal::mask::wait_for_read;
			return s;
		} else if (serr == 0) {
			s.m = reactor::signal::mask::switch_protocols;
			s.protocol_switch = pi->instantiate();
			return s;
		} else {
			throw exception::connection_establish {};
		}
		
		return s;
	}
	virtual reactor::signal::mask::type default_mask() override { return reactor::signal::mask::wait_for_write; }
	addrinfo * addr = nullptr;
	std::shared_ptr<reactor::protocol_instantiator> pi;
};

void reactor::connect(std::string const & host, std::string const & service, std::shared_ptr<protocol_instantiator> const & pi) {
	
	std::unique_ptr<connection_protocol> cprot = std::make_unique<connection_protocol>();
	cprot->pi = pi;
	
	static constexpr addrinfo hints = {
		.ai_flags = 0,
		.ai_family = AF_UNSPEC,
		.ai_socktype = SOCK_STREAM,
		.ai_protocol = 0,
		.ai_addrlen = 0,
		.ai_addr = nullptr,
		.ai_canonname = nullptr,
		.ai_next = nullptr
	};
	
	socket sock;
	
	int status = getaddrinfo(host.c_str(), service.c_str(), &hints, &cprot->addr);
	if (status) throw exception::connection_resolve {};
	
	for (addrinfo * ai = cprot->addr; ai != nullptr; ai = ai->ai_next) {
		if (ai->ai_family == AF_INET) sock.FD = ::socket(PF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
		else if (ai->ai_family == AF_INET6) sock.FD = ::socket(PF_INET6, SOCK_STREAM | SOCK_NONBLOCK, 0);
		else continue;
		
		status = ::connect(sock.FD, ai->ai_addr, ai->ai_addrlen);
		if (status == -1 && errno != EINPROGRESS) {
			throw exception::connection_resolve {};
		}
		break;
	}
	
	accept_connection(std::move(sock), std::move(cprot));
}
