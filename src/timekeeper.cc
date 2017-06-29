#include "asterid/timekeeper.hh"

#include <thread>

asterid::timekeeper::timekeeper(double FPS) : target{1/FPS} {
	frame = clock::now();
}

void asterid::timekeeper::sleep_for_target() {
	clock::time_point frame2 = clock::now();
	target_t frame_time = frame2 - frame;
	if (frame_time < target) {
		target_t sleep_time = target - frame_time;
		std::this_thread::sleep_for(sleep_time);
	}
	frame = clock::now();
}

void asterid::timekeeper::set_target(double FPS) {
	target = target_t {1/FPS};
}
