#pragma once

#include "buffer_assembly.hh"
#include "synchro.hh"
#include "time.hh"

#include <atomic>
#include <condition_variable>
#include <forward_list>
#include <functional>
#include <memory>
#include <queue>
#include <thread>
#include <vector>

#include <stdexcept>

#include <netinet/in.h>

namespace asterales::cicada {
	
	struct socket;
	struct connection;
	
	namespace exception {
		struct base {};
		struct connection_resolve : public base {};
		struct connection_establish : public base {};
		struct socket_acquire : public base {};
		struct socket_bind : public base {};
		struct listen_start : public base {};
		struct sendfile_notfound : public base {};
		struct sendfile_notfile : public base {};
		struct sendfile_badoffset : public base {};
		
		typedef std::runtime_error generic;
	}
	
	struct socket {
		int FD;
		
		struct sockaddr_in6 addr;
		
		socket() = default;
		virtual ~socket();
		
		void close();
	};
	
	struct sendfile_helper {
		sendfile_helper(std::string const & path, off_t offset, size_t count);
		bool is_done();
		ssize_t work(connection &);
	private:
		struct impl_t;
		std::unique_ptr<impl_t> impl;
	};
	
	struct connection : public socket {
		connection() = delete;
		connection(connection const & other) = delete;
		connection(connection &&);
		connection(socket &&);
		virtual ~connection() = default;
		
		ssize_t read(char * buf, size_t buf_len); // 1:1 recv
		ssize_t read(buffer_assembly &, size_t cnt = SIZE_MAX); // read up to <cnt> bytes into a byte buffer, appends to end
		ssize_t write(char const * buf, size_t buf_len); // 1:1 send
		ssize_t write(buffer_assembly const &, size_t cnt = SIZE_MAX); // write up to <cnt> bytes, does not modify buffer
		ssize_t write_consume(buffer_assembly &, size_t cnt = SIZE_MAX); // write up to <cnt> bytes, consumes from beginning
		ssize_t sendfile(int fd, off_t * offs, size_t size); // 1:1 sendfile
		ssize_t sendfile(sendfile_helper &);
	};
	
	struct listener : public socket {
		typedef std::function<void(socket &&)> accept_cb;
		
		listener() = delete;
		listener(listener const &) = delete;
		listener(listener &&) = delete;
		listener(uint16_t, accept_cb &&);
		virtual ~listener() = default;
		
		void accept() noexcept;
		
	private:
		accept_cb _accept_cb;
	};
	
	struct reactor final {
		
		struct reason {
			typedef uint_fast8_t type;
			static constexpr type read_available = 1 << 0;
			static constexpr type write_available = 1 << 1;
			static constexpr type pulse = 1 << 2;
		};
	
		struct detail {
			reason::type ready_reason;
		};
		
		struct protocol;
		
		struct signal {
		
			struct mask {
				typedef uint_fast8_t type;
				static constexpr type terminate = 1 << 0;
				static constexpr type wait_for_read = 1 << 1;
				static constexpr type wait_for_write = 1 << 2;
				static constexpr type switch_protocols = 1 << 3;
			};
			
			mask::type m {0};
			std::chrono::milliseconds wait_time {0};
			std::unique_ptr<protocol> protocol_switch;
		};
		
		typedef std::function<void(signal::mask::type)> setmask_cb;
		
		struct protocol {
			setmask_cb set_mask;
			virtual ~protocol() = default;
			virtual signal ready(connection &, detail const &) = 0;
			virtual signal::mask::type default_mask() { return signal::mask::wait_for_read | signal::mask::wait_for_write; }
		};
		
		reactor(bool create_master_thread = false, unsigned int workers = std::thread::hardware_concurrency());
		reactor(uint16_t port, bool create_master_thread = true, unsigned int workers = std::thread::hardware_concurrency());
		reactor(reactor const &) = delete;
		reactor(reactor &&) = delete;
		~reactor();
		
		struct protocol_instantiator {
			virtual ~protocol_instantiator() = default;
			virtual std::unique_ptr<protocol> instantiate() = 0;
		};
		
		typedef std::shared_ptr<protocol_instantiator> protocol_instantiator_ptr;
		
		void listen(uint16_t port, std::shared_ptr<protocol_instantiator> const & pi_in) {
			service_lock.lock();
			services[port] = std::unique_ptr<listener> { new listener { port, [this, pi = pi_in] (connection && con) mutable { this->accept_connection(std::forward<connection &&>(con), pi); } } };
			epoll_register(services[port]->FD);
			service_lock.unlock();
		}
		template <typename T> void listen(uint16_t port) { listen(port, std::shared_ptr<automatic_protocol_instantiator<T>> { new automatic_protocol_instantiator<T> {} }); }
		
		void master(std::function<bool()> pred); // when passing false for create_master_thread, an existing thread must act as the master by calling this function, a predicate is passed to be able to stop mastering at any point
		
		inline void accept_connection(connection && con, std::shared_ptr<protocol_instantiator> const & pi) {
			this->accept_connection(std::forward<connection &&>(con), pi->instantiate());
		}
		void accept_connection(connection && con, std::unique_ptr<protocol> && pi) {
			int d = con.FD;
			instance_lock.write_lock();
			instances[d] = std::shared_ptr<instance> { new instance { *this, std::forward<connection>(con), std::forward<std::unique_ptr<protocol> &&>(pi) } };
			instance_lock.write_unlock();
 		}
 		
 		template <typename P> inline void connect(std::string const & host, std::string const & service) {
			connect(host, service, std::make_unique<automatic_protocol_instantiator<P>>());
		}
 		void connect(std::string const & host, std::string const & service, std::shared_ptr<protocol_instantiator> const & pi);
		
	private:
		
		struct instance {
			instance(reactor const & parent, connection && con, std::unique_ptr<protocol> && pr);
			~instance();
			reactor const & parent;
			connection con;
			std::unique_ptr<protocol> proto;
			asterales::spinlock use_lock;
			void * epoll_evt;
			void update_epoll(int flags);
		};
		
		template <typename T> struct automatic_protocol_instantiator : public protocol_instantiator {
			virtual std::unique_ptr<protocol> instantiate() override { return std::unique_ptr<protocol> { new T {} }; }
		};
		
		std::unordered_map<uint16_t, std::unique_ptr<listener>> services;
		asterales::spinlock service_lock;
		
		std::unordered_map<int, std::shared_ptr<instance>> instances;
		asterales::rw_spinlock instance_lock;
		
		std::atomic_bool run_sem {true};
		std::thread * master_thread = nullptr;
		std::vector<std::thread *> workers;
		
		struct m2w_msg {
			inline m2w_msg(int d_, reason::type r_) : descriptor {d_}, r {r_} {}
			int descriptor;
			reason::type r;
		};
		
		std::queue<m2w_msg> m2w_queue;
		asterales::spinlock m2w_lock;
		std::condition_variable m2w_cv;
		std::mutex m2w_cv_mut;
		
		asterales::time::point last_pulse;
		
		int epoll_obj;
		void * epoll_mevt;
		void epoll_register(int fd);
		
		void master_loop();
		void worker_run();
	};
	
}
