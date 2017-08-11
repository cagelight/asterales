#pragma once

#include <functional>
#include <string>
#include <vector>
#include <cstdint>

namespace asterid {
	
	static std::string const empty_str {};
	
	std::string strf( char const * fmt, ... ) noexcept;
	char const * vas( char const * fmt, ... ) noexcept;
	
}

#define _as_here asterid::vas("%s, in %s, line %u", __PRETTY_FUNCTION__, __FILE__,  __LINE__)
#define _as_herestr asterid::strf("%s, in %s, line %u", __PRETTY_FUNCTION__, __FILE__,  __LINE__)
