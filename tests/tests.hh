#pragma once

#include <cassert>
#include <string>

#include "asterid/strop.hh"

#define TEST(cond) assert(cond)

namespace tests {
	void buffer_assembly_tests();
	void threadpool_tests();
	void codon_tests();
	void brassica_tests();
	void strop_tests();
}

namespace util {
	enum struct log_level {
		info,
		warning,
		error,
	};
	void log(log_level, std::string const &);
	struct cilogger {
		util::log_level level;
		std::string str;
		inline cilogger(util::log_level lev, std::string const & init = "") : level(lev), str(init) {}
		inline ~cilogger() { util::log(level, str); }
		
		inline cilogger & operator << ( std::string const & other ) { str += other; return *this; }
		inline cilogger & operator << ( char const * other ) { str += other; return *this; }
		template <typename T> inline cilogger & operator << ( T const & other ) { str += std::to_string(other); return *this; }
	};
}

#define tlog asterid::streamlogger {"", [](std::string const & str){ printf("%s\n", str.c_str()); }}
#define tlogi asterid::streamlogger {"", [](std::string const & str){ printf("%s", str.c_str()); fflush(stdout); }}
