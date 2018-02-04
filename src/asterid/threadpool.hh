#pragma once

#include <atomic>
#include <condition_variable>
#include <functional>
#include <future>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

namespace asterid {
	
	struct task_base {
		virtual void execute() = 0;
	};
	
	template <typename R, typename L> struct task_lambda_void : public task_base {
		L lambda;
		std::promise<void> promise;
		task_lambda_void(L && lambda) : lambda(std::forward<L &&>(lambda)) {}
		inline std::future<R> get_future() { return promise.get_future(); }
		virtual void execute() override {
			lambda();
			promise.set_value();
		}
	};
	
	template <typename R, typename L> struct task_lambda_nvoid : public task_base {
		L lambda;
		std::promise<R> promise;
		task_lambda_nvoid(L && lambda) : lambda(std::forward<L &&>(lambda)) {}
		inline std::future<R> get_future() { return promise.get_future(); }
		virtual void execute() override {
			promise.set_value(lambda());
		}
	};
	
	template <typename R, typename L, std::enable_if_t<std::is_void<R>::value, int> = 0> std::unique_ptr<task_lambda_void<R, L>> task_lambda (L && lambda) {
		return std::make_unique<task_lambda_void<R, L>>(std::forward<L &&>(lambda));
	}
	template <typename R, typename L, std::enable_if_t<!std::is_void<R>::value, int> = 0> std::unique_ptr<task_lambda_nvoid<R, L>> task_lambda (L && lambda) {
		return std::make_unique<task_lambda_nvoid<R, L>>(std::forward<L &&>(lambda));
	}
	
	struct thread_pool {
		
		thread_pool(size_t pool_size = std::thread::hardware_concurrency());
		~thread_pool();
		
		void enqueue(std::unique_ptr<task_base> &&);
		
	private:
		
		std::atomic_bool run_sem {true};
		std::condition_variable queue_cv;
		std::mutex queue_m, cv_m;
		std::vector<std::thread> threads;
		
		std::queue<std::unique_ptr<task_base>> task_queue;
		
		void thread_run();
	};
	
	
	
}
