#include "tests.hh"

#include "asterales/threadpool.hh"

void tests::threadpool_tests() {
	asterales::thread_pool tpt;
	int i = 0;
	
	auto t1 = asterales::task_lambda<void>([&i](){ i += 1; });
	auto t1f = t1->get_future();
	tpt.enqueue(std::move(t1));
	
	auto t2 = asterales::task_lambda<int>([&i](){ i += 1; return i; });
	auto t2f = t2->get_future();
	tpt.enqueue(std::move(t2));
	
	t1f.get();
	t2f.get();
	
	printf("%i\n", i);
	
	/*
	std::vector<unsigned char> pixels;
	auto taskF = tpt.enqueue<void>([](std::string folderPath, uint32_t width, uint32_t height, std::vector<unsigned char> &&pixels)
    {
	}, std::string("TEST"), 32, 32, std::move(pixels));
	*/
}
