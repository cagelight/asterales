#include "asterid/byte_buffer.hh"

static constexpr uint8_t hmask = 0b11110000;
static constexpr uint8_t lmask = 0b00001111;

std::string asterid::byte_buffer::hex() const {
	std::string ret {};
	ret.reserve(buf_.size() * 2);
	for (uint8_t ch : buf_) {
		char vh = (ch & hmask) >> 4;
		char vl = ch & lmask;
		ret += vh > 9 ? 55 + vh : 48 + vh;
		ret += vl > 9 ? 55 + vl : 48 + vl;
	}
	return ret;
}

std::string asterid::byte_buffer::hexlow() const {
	std::string ret {};
	ret.reserve(buf_.size() * 2);
	for (uint8_t ch : buf_) {
		char vh = (ch & hmask) >> 4;
		char vl = ch & lmask;
		ret += vh > 9 ? 87 + vh : 48 + vh;
		ret += vl > 9 ? 87 + vl : 48 + vl;
	}
	return ret;
}

