#pragma once

#include <cassert>
#include <string>

#define TEST(cond) assert(cond)

namespace tests {
	void buffer_assembly_tests();
	void threadpool_tests();
	void codon_tests();
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

#define scilog(prefix, lev) util::cilogger { lev, #prefix ": " }
#define scilogv(prefix, lev) util:::cilogger { lev, asterid::strf("%s (%s): ", #prefix, _as_here) }
#define scilogi scilog(INFO, util::log_level::info)
#define scilogvi scilogv(INFO, util::log_level::info)
#define scilogw scilog(WARNING, util::log_level::warning)
#define scilogvw scilogv(WARNING, util::log_level::warning)
#define sciloge scilog(ERROR, util::log_level::error)
#define scilogve scilogv(ERROR, util::log_level::error)
