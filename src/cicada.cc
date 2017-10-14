#include "asterid/cicada.hh"

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

using namespace asterid::cicada;

static constexpr int enable = 1;
static constexpr int disable = 0;

#define TMPBUF_SIZE 512

#define EPOLLEVT reinterpret_cast<epoll_event *>(epoll_evt)

socket::~socket() {
	if (FD != -1) {
		shutdown(FD, SHUT_RDWR);
		close(FD);
	}
}

struct sendfile_helper::private_data {
	int fd;
	struct stat finfo;
	off_t offset;
	size_t count;
	private_data(std::string const & path, off_t offset, size_t count) : offset(offset), count(count) {
		fd = ::open(path.c_str(), O_RDONLY);
		if (fd < 0) throw exception::sendfile_notfound {};
		fstat(fd, &finfo);
		if (!S_ISREG(finfo.st_mode)) throw exception::sendfile_notfile {};
		if (offset > finfo.st_size) throw exception::sendfile_badoffset {};
		if (offset + count > static_cast<size_t>(finfo.st_size)) count = finfo.st_size - offset;
	}
};

sendfile_helper::sendfile_helper(std::string const & path, off_t offset, size_t count) {
	data.reset(new private_data {path, offset, count});
}

bool sendfile_helper::is_done() {
	return !data->count;
}

ssize_t sendfile_helper::work(connection & con) {
	ssize_t amt = con.sendfile(data->fd, &data->offset, data->count);
	if (amt < 0) return -1;
	data->count -= amt;
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

listener::listener(uint16_t port, accept_cb cb) : _accept_cb(cb) {
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

void listener::accept() {
	while (true) {
		socket temp;
		socklen_t slen = sizeof(temp.addr);
		temp.FD = accept4(FD, reinterpret_cast<struct sockaddr *>(&temp.addr), &slen, SOCK_NONBLOCK);
		if (temp.FD == -1) return;
		_accept_cb(std::move(temp));
	}
}

server::server(uint16_t port, bool create_master_thread, unsigned int workers_num) : li(port, [this](connection && con){ this->accept_connection(std::forward<connection>(con)); }) {
	epoll_obj = epoll_create(1);
	epoll_evt = new epoll_event {};
	EPOLLEVT->data.fd = li.FD;
	EPOLLEVT->events = EPOLLIN;
	epoll_ctl(epoll_obj, EPOLL_CTL_ADD, li.FD, EPOLLEVT);
	for (unsigned int i = 0; i < workers_num; i++) workers.push_back( new std::thread { &server::worker_run, this } );
	if (create_master_thread) master_thread = new std::thread { [this](){ while (run_sem) master_loop(); } };
}

server::~server() {
	run_sem.store(false);
	if (master_thread) {
		if (master_thread->joinable()) master_thread->join();
		delete master_thread;
	}
	m2w_cv.notify_all();
	for (std::thread * worker : workers) {
		if (worker->joinable()) worker->join();
		delete worker;
	}
	if (pib) delete pib;
	close(epoll_obj);
	if (EPOLLEVT) delete EPOLLEVT;
}

void server::master(std::function<bool()> pred) {
	while (pred()) master_loop();
}

void server::accept_connection(connection && con) {
	instances.emplace_front(new instance { *this, std::forward<connection>(con), pib->instantiate() });
}

void server::master_loop() {
	epoll_event event {};
	epoll_wait(epoll_obj, &event, 1, 100);
	li.accept();
	
	w2m_lock.lock();
	while (!w2m_queue.empty()) {
		auto msg = w2m_queue.front();
		w2m_queue.pop();
		
		std::shared_ptr<instance> inst = msg.i.lock();
		if (!inst) continue;
		if (msg.sig.m & signal::mask::terminate) {
			instances.remove(inst);
			inst.reset();
		}
	}
	w2m_lock.unlock();
	for (auto & i : instances) {
		m2w_lock.lock();
		m2w_queue.emplace(i, reason::pulse);
		m2w_lock.unlock();
	}
	m2w_cv.notify_all();
}
void server::worker_run() {
	while (run_sem) {
		{
			std::unique_lock<std::mutex> lk {m2w_cv_mut};
			m2w_cv.wait_for(lk, std::chrono::milliseconds(1000));
		}
		
		if (!run_sem) return;
		
		while (true) {
			m2w_lock.lock();
			if (m2w_queue.empty()) {
				m2w_lock.unlock();
				break;
			}
			auto msg = m2w_queue.front();
			m2w_queue.pop();
			m2w_lock.unlock();
			
			std::shared_ptr<instance> inst = msg.i.lock();
			if (!inst) continue;
			if (!inst->use_lock.try_lock()) continue;
			detail d { msg.r };
			
			signal sig {};
			
			try {
				sig = inst->proto->ready(inst->con, d);
			} catch (...) {
				printf("WARNING: a connection was terminated after catching an uncaught exception\n");
				sig.m |= signal::mask::terminate;
			}
			
			int flags = 0;
			if (sig.m & signal::mask::wait_for_read) flags |= EPOLLIN;
			if (sig.m & signal::mask::wait_for_write) flags |= EPOLLOUT;
			inst->update_epoll(flags);
			
			inst->use_lock.unlock();
			
			w2m_lock.lock();
			w2m_queue.emplace(msg.i, std::move(sig));
			w2m_lock.unlock();
		}
	}
}

server::instance::instance(server const & parent, connection && con, protocol * proto) : parent(parent), con(std::forward<connection>(con)), proto(proto) {
	epoll_evt = new epoll_event {};
	EPOLLEVT->data.fd = this->con.FD;
	EPOLLEVT->events = EPOLLIN;
	epoll_ctl(parent.epoll_obj, EPOLL_CTL_ADD, this->con.FD, EPOLLEVT);
}
server::instance::~instance() {
	epoll_ctl(parent.epoll_obj, EPOLL_CTL_DEL, con.FD, EPOLLEVT);
	if (EPOLLEVT) delete EPOLLEVT;
}

void server::instance::update_epoll(int flags) {
	EPOLLEVT->events = flags;
	epoll_ctl(parent.epoll_obj, EPOLL_CTL_MOD, con.FD, EPOLLEVT);
}
