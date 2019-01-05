#include "asterales/threadpool.hh"

asterales::thread_pool::thread_pool(size_t pool_size) {
	for (size_t i = 0; i < pool_size; i++) {
		threads.emplace_back(&thread_pool::thread_run, this);
	}
}

asterales::thread_pool::~thread_pool() {
	run_sem.store(false);
	queue_cv.notify_all();
	for (std::thread & th : threads) {
		if (th.joinable()) th.join();
	}
}

void asterales::thread_pool::enqueue(std::unique_ptr<task_base> && task) {
	std::unique_lock lk {queue_m};
	task_queue.push(std::forward<std::unique_ptr<task_base> &&>(task));
	queue_cv.notify_one();
}

void asterales::thread_pool::thread_run() {
	while (run_sem) {
		{
			std::unique_lock lk {cv_m};
			queue_cv.wait_for(lk, std::chrono::milliseconds(5000));
		}
		{
			std::unique_lock lk {queue_m};
			if (task_queue.empty()) continue;
			auto task = std::move(task_queue.front());
			task_queue.pop();
			lk.unlock();
			task->execute();
		}
	}
}
