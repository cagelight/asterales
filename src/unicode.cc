#include "asterid/unicode.hh"
#include <unicode/ustring.h>
#include <cstring>

using namespace asterid;
using namespace asterid::unicode;

ustring::ustring(char const * utf8, size_t size) {
	UChar32 c = 0;
	uint8_t const * rstr = reinterpret_cast<uint8_t const *>(utf8);
	int32_t offs = 0;
	while (true) {
		U8_NEXT(rstr, offs, size, c);
		if (c < 0) break;
		push_back(c);
	}
}

std::string ustring::to_string() const {
	// TODO
	return {};
}
