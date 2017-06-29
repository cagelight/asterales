#pragma once

#include <functional>
#include <string>
#include <vector>
#include <cstdint>

#define VAS_HISTORY_SIZE 8

namespace asterid {
	
	std::string strf( char const * fmt, ... ) noexcept;
	char const * vas( char const * fmt, ... ) noexcept;
	
}

#define here asterid::vas("%s, in %s, line %u", __PRETTY_FUNCTION__, __FILE__,  __LINE__)
#define herestr asterid::strf("%s, in %s, line %u", __PRETTY_FUNCTION__, __FILE__,  __LINE__)
