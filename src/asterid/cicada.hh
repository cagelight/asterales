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

namespace asterid::cicada {
	
	struct socket;
	struct connection;
	
	namespace exception {
		struct base {};
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
		struct private_data;
		std::unique_ptr<private_data> data;
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
	
	struct server final {
		
		struct reason {
			typedef uint_fast8_t type;
			static constexpr type read_available = 1 << 0;
			static constexpr type write_available = 1 << 1;
			static constexpr type pulse = 1 << 2;
		};
	
		struct detail {
			reason::type ready_reason;
		};
		
		struct signal {
		
			struct mask {
				typedef uint_fast8_t type;
				static constexpr type terminate = 1 << 0;
				static constexpr type wait_for_read = 1 << 1;
				static constexpr type wait_for_write = 1 << 2;
			};
			
			mask::type m {0};
			std::chrono::milliseconds wait_time {0};
		};
		
		typedef std::function<void(signal::mask::type)> setmask_cb;
		
		struct protocol {
			setmask_cb set_mask;
			virtual ~protocol() = default;
			virtual signal ready(connection &, detail const &) = 0;
		};
		
		server() = delete;
		server(bool create_master_thread = true, unsigned int workers = std::thread::hardware_concurrency());
		server(uint16_t port, bool create_master_thread = true, unsigned int workers = std::thread::hardware_concurrency());
		server(server const &) = delete;
		server(server &&) = delete;
		~server();
		
		struct protocol_instantiator {
			virtual ~protocol_instantiator() = default;
			virtual std::unique_ptr<protocol> instantiate() = 0;
		};
		
		struct parent_interface {
			
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
		
		void accept_connection(connection && con, std::shared_ptr<protocol_instantiator> const & pi) {
			int d = con.FD;
			instance_lock.write_lock();
			instances[d] = std::shared_ptr<instance> { new instance { *this, std::forward<connection>(con), pi } };
			instance_lock.write_unlock();
 		}
		
	private:
		
		struct instance {
			instance(server const & parent, connection && con, std::shared_ptr<protocol_instantiator> const & pi);
			~instance();
			server const & parent;
			connection con;
			std::unique_ptr<protocol> proto;
			asterid::spinlock use_lock;
			void * epoll_evt;
			void update_epoll(int flags);
		};
		
		template <typename T> struct automatic_protocol_instantiator : public protocol_instantiator {
			virtual std::unique_ptr<protocol> instantiate() override { return std::unique_ptr<protocol> { new T {} }; }
		};
		
		std::unordered_map<uint16_t, std::unique_ptr<listener>> services;
		asterid::spinlock service_lock;
		
		//std::forward_list<std::shared_ptr<instance>> instances;
		std::unordered_map<int, std::shared_ptr<instance>> instances;
		asterid::rw_spinlock instance_lock;
		
		std::atomic_bool run_sem {true};
		std::thread * master_thread = nullptr;
		std::vector<std::thread *> workers;
		
		struct m2w_msg {
			inline m2w_msg(int d_, reason::type r_) : descriptor {d_}, r {r_} {}
			int descriptor;
			reason::type r;
		};
		
		std::queue<m2w_msg> m2w_queue;
		asterid::spinlock m2w_lock;
		std::condition_variable m2w_cv;
		std::mutex m2w_cv_mut;
		
		asterid::time::point last_pulse;
		
		int epoll_obj;
		void * epoll_mevt;
		void epoll_register(int fd);
		
		void master_loop();
		void worker_run();
	};
	
}
