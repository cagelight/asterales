#pragma once

#include <cstring>
#include <string>
#include <vector>

// NOTE -- UTF-16 is dumb and anybody who uses it is dumb

namespace asterid::unicode {
	
	typedef char32_t uchar;
	
	// ustring is a UTF-32 encoded Unicode string, meant to be iterated over for font libraries like FreeType2
	// WARNING -- ustring is NOT null terminated, it cannot be used with C string functions (it wouldn't be able even if it was null terminated)
	struct ustring : public std::vector<uchar> {
		inline ustring() : vector<uchar>() {}
		ustring(char const * utf8, size_t size);
		ustring(char const * utf8) : ustring(utf8, strlen(utf8)) {}
		ustring(std::string const & utf8) : ustring(utf8.c_str(), utf8.size()) {}
		inline ustring(ustring const & other) = default;
		inline ustring(ustring && other) = default;
		
		std::string to_string() const; // UTF-8 encoded
	};
	
}

namespace std {
	inline std::string to_string(asterid::unicode::ustring const & ustr) { return ustr.to_string(); }
}
