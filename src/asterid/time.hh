#pragma once

#include <ctime>
#include <ctgmath>

#include <cstdio>

#define MILLI 1000
#define MICRO 1000000
#define NANO 1000000000

namespace asterid::time {
	
	struct unit : public timespec {
		
		unit() = default;
		unit(timespec const & t) : timespec(t) {}
		
		template <typename T> static T add(unit const & A, unit const & B) {
			T ret;
			ret.tv_sec = A.tv_sec + B.tv_sec;
			ret.tv_nsec = A.tv_nsec + B.tv_nsec;
			ret.resolve();
			return ret;
		}
		
		template <typename T> static T subtract(unit const & A, unit const & B) {
			T ret;
			ret.tv_sec = A.tv_sec - B.tv_sec;
			ret.tv_nsec = A.tv_nsec - B.tv_nsec;
			ret.resolve();
			return ret;
		}
		
	protected:
		void resolve() {
			auto mval = tv_nsec / NANO;
			if (tv_nsec < 0) mval -= 1;
			tv_nsec -= mval * NANO;
			tv_sec += mval;
		}
	};
	
	struct span : public unit {
		
		span() = default;
		span(double seconds) {
			tv_sec = floor(seconds);
			seconds -= tv_sec;
			tv_nsec = seconds * NANO;
			resolve();
		}

		template <typename T = double> T sec() const {
			T val = tv_sec;
			val += tv_nsec / static_cast<T>(NANO);
			return val;
		}
		
		template <typename T> T msec() const {
			T val = tv_sec * static_cast<T>(MILLI);
			val += tv_nsec / static_cast<T>(NANO / MILLI);
			return val;
		}
		
		inline span operator + (span const & other) const { return add<span>(*this, other); }
		inline span operator - (span const & other) const { return subtract<span>(*this, other); }
	};
	
	struct point : public unit {

		point() = default;
		
		inline span operator - (point const & other) const { return subtract<span>(*this, other); }
		
		inline point operator + (span const & other) const { return add<point>(*this, other); }
		inline point operator - (span const & other) const { return subtract<point>(*this, other); }
	};
	
	enum struct clock_type : clockid_t {
		realtime = CLOCK_REALTIME,
		monotonic = CLOCK_MONOTONIC,
		process = CLOCK_PROCESS_CPUTIME_ID,
		thread = CLOCK_THREAD_CPUTIME_ID,
	};
	
	template <clock_type CT> point now() {
		point p;
		clock_gettime(static_cast<clockid_t>(CT), &p);
		return p;
	}
	
	template <clock_type CT> struct keeper final {
		
		point last_mark;
		
		keeper() = default;
		
		span mark() {
			point n = now<CT>();
			span s = n - last_mark;
			last_mark = n;
			return s;
		}
		
		span mark_ghost() const {
			return now<CT>() - last_mark;
		}
		
		span sleep_mark(span const & total) { // total includes time since last mark
			span cs = total - mark_ghost();
			if (cs.tv_sec >= 0) nanosleep(&cs, nullptr);
			return mark();
		}
	};
	
}
