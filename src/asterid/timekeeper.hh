#pragma once

#include <chrono>

namespace asterid {
	
	struct timekeeper final {
		
		typedef std::chrono::duration<double, std::ratio<1, 1>> target_t;
		typedef std::chrono::steady_clock clock;
		
		target_t target;
		clock::time_point frame;
		
		timekeeper(double FPS = 60);
		~timekeeper() = default;
		
		inline void reset() { frame = clock::now(); }
		void set_target(double FPS);
		void sleep_for_target();
		
	};
	
}
