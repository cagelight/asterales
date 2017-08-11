#pragma once

#include "byte_buffer.hh"
#include "synchro.hh"

#include <atomic>
#include <condition_variable>
#include <forward_list>
#include <functional>
#include <memory>
#include <queue>
#include <thread>
#include <vector>

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
	}
	
	struct socket {
		int FD;
		struct sockaddr_in6 addr;
		
		socket() = default;
		virtual ~socket();
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
		ssize_t read(byte_buffer &, size_t cnt = SIZE_MAX); // read up to <cnt> bytes into a byte buffer, appends to end
		ssize_t write(char const * buf, size_t buf_len); // 1:1 send
		ssize_t write(byte_buffer const &, size_t cnt = SIZE_MAX); // write up to <cnt> bytes, does not modify buffer
		ssize_t write_consume(byte_buffer &, size_t cnt = SIZE_MAX); // write up to <cnt> bytes, consumes from beginning
		ssize_t sendfile(int fd, off_t * offs, size_t size); // 1:1 sendfile
		ssize_t sendfile(sendfile_helper &);
	};
	
	struct listener : public socket {
		typedef std::function<void(socket &&)> accept_cb;
		
		listener() = delete;
		listener(listener const &) = delete;
		listener(listener &&) = delete;
		listener(uint16_t, accept_cb);
		virtual ~listener() = default;
		
		void accept();
		
	private:
		accept_cb _accept_cb;
	};
	
	struct server final {
		
		struct reason {
			typedef uint_fast8_t type;
			static constexpr type read_available = 1 << 0;
			static constexpr type write_available = 1 << 1;
			static constexpr type pulse = 1 << 2;
			static constexpr type timeout = 1 << 3;
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
				static constexpr type wait_for_time = 1 << 3;
			};
			
			mask::type m {0};
			std::chrono::milliseconds wait_time {0};
		};
		
		struct protocol {
			virtual ~protocol() = default;
			virtual signal ready(connection &, detail const &) = 0;
		};
		
		server() = delete;
		server(uint16_t port, bool create_master_thread = true, unsigned int workers = std::thread::hardware_concurrency());
		server(server const &) = delete;
		server(server &&) = delete;
		~server();
		
		template <typename PROTO> void begin() {
			if (pib) delete pib;
			pib = new protocol_instantiator<PROTO> {};
		}
		
		void master(std::function<bool()> pred); // when passing false for create_master_thread, an existing thread must act as the master by calling this function, a predicate is passed to be able to stop mastering at any point
		
	private:
		
		struct protocol_instantiator_base {
			virtual ~protocol_instantiator_base() = default;
			virtual protocol * instantiate() = 0;
		};
		
		template <typename PROTO> struct protocol_instantiator final : public protocol_instantiator_base {
			virtual protocol * instantiate() override {
				return new PROTO {};
			}
		};
		
		struct instance {
			instance(connection && con, protocol * proto);
			connection con;
			protocol * proto;
			asterid::spinlock use_lock;
		};
		
		listener li;
		protocol_instantiator_base * pib = nullptr;
		std::forward_list<std::shared_ptr<instance>> instances;
		
		std::atomic_bool run_sem {true};
		std::thread * master_thread = nullptr;
		std::vector<std::thread *> workers;
		
		struct m2w_msg {
			inline m2w_msg(std::shared_ptr<instance> & _i, reason::type _r) : i(_i), r(_r) {}
			std::weak_ptr<instance> i;
			reason::type r;
		};
		
		std::queue<m2w_msg> m2w_queue;
		asterid::spinlock m2w_lock;
		std::condition_variable m2w_cv;
		std::mutex m2w_cv_mut;
		
		struct w2m_msg {
			inline w2m_msg(std::weak_ptr<instance> & _i, signal && _sig) : i(_i), sig(std::forward<signal>(_sig)){}
			std::weak_ptr<instance> i;
			signal sig;
		};
		
		std::queue<w2m_msg> w2m_queue;
		asterid::spinlock w2m_lock;
		
		void accept_connection(connection &&);
		void master_loop();
		void worker_run();
	};
	
}
