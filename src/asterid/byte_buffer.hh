#pragma once
 
// GENERAL PURPOSE BYTE BUFFER

#include <cstdint>
#include <cstring>
#include <string>
#include <vector>

namespace asterid {
	
	struct byte_buffer final {

		typedef std::vector<uint8_t>::iterator iterator;
		typedef std::vector<uint8_t>::const_iterator const_iterator;
		
		byte_buffer() = default;
		byte_buffer(byte_buffer const & other) = default;
		byte_buffer(byte_buffer && other) = default;
		inline byte_buffer(byte_buffer::const_iterator const & start, byte_buffer::const_iterator const & end) : buf_(start, end) {}
		inline byte_buffer(std::string const & str) { buf_.resize(str.size()); memcpy(buf_.data(), str.data(), str.size()); }
		inline byte_buffer(char const * dat, size_t len) { buf_.resize(len); memcpy(buf_.data(), dat, len); }
		~byte_buffer() = default;

		std::string hex () const;
		std::string hexlow () const;
		inline size_t size () const { return buf_.size(); }
		inline uint8_t * data () { return buf_.data(); }
		inline uint8_t const * data () const { return buf_.data(); }
		inline void reserve (size_t s) { buf_.reserve(s); }
		inline void resize (size_t s) { buf_.resize(s); }
		inline void clear() { buf_.clear(); }
		inline void pop(size_t cnt = 1) { if (!cnt) return; *this = byte_buffer { begin() + cnt, end() }; }
		inline iterator begin () { return buf_.begin(); }
		inline iterator end () { return buf_.end(); }
		inline const_iterator begin () const { return buf_.begin(); }
		inline const_iterator end () const { return buf_.end(); }
		inline iterator insert(const_iterator P, const_iterator A, const_iterator B) { return buf_.insert(P, A, B); }
		inline std::string to_string() const { return std::string { reinterpret_cast<char const *>(data()), size() }; }
		
		inline size_t transfer_to(byte_buffer & other, size_t cnt = SIZE_MAX) {
			if (cnt > size()) cnt = size();
			if (cnt) {
				other.insert(other.end(), begin(), begin() + cnt);
				pop(cnt);
			}
			return cnt;
		}
		
		inline byte_buffer & operator = (byte_buffer const & other) = default;
		inline byte_buffer & operator = (byte_buffer && other) = default;
		
		inline byte_buffer operator + (byte_buffer const & other) const { byte_buffer bb {*this}; bb.insert(bb.end(), other.begin(), other.end()); return bb; }
		
		inline byte_buffer & operator << (byte_buffer const & other) { this->insert(this->end(), other.begin(), other.end()); return *this; }
		inline byte_buffer & operator << (std::string const & other) { buf_.insert(buf_.end(), other.c_str(), other.c_str() + other.size()); return *this; }
		inline byte_buffer & operator << (char const * other) { buf_.insert(buf_.end(), other, other + strlen(other)); return *this; }
		
		#define _asterid_bb_sin(TYPE) inline byte_buffer & operator << (TYPE v) { buf_.insert(buf_.end(), &v, &v + sizeof(TYPE)); return *this; }
		_asterid_bb_sin(int8_t)
		_asterid_bb_sin(uint8_t)
		_asterid_bb_sin(int16_t)
		_asterid_bb_sin(uint16_t)
		_asterid_bb_sin(int32_t)
		_asterid_bb_sin(uint32_t)
		_asterid_bb_sin(int64_t)
		_asterid_bb_sin(uint64_t)
		_asterid_bb_sin(float)
		_asterid_bb_sin(double)
		_asterid_bb_sin(long double)
		
		#define _asterid_bb_sout(TYPE) inline byte_buffer & operator >> (TYPE & v) { v = *reinterpret_cast<TYPE *>(buf_.data()); pop(sizeof(TYPE)); return *this; }
		_asterid_bb_sout(int8_t)
		_asterid_bb_sout(uint8_t)
		_asterid_bb_sout(int16_t)
		_asterid_bb_sout(uint16_t)
		_asterid_bb_sout(int32_t)
		_asterid_bb_sout(uint32_t)
		_asterid_bb_sout(int64_t)
		_asterid_bb_sout(uint64_t)
		_asterid_bb_sout(float)
		_asterid_bb_sout(double)
		_asterid_bb_sout(long double)
		
		inline std::vector<uint8_t> & get_buffer() {return buf_;}
		inline std::vector<uint8_t> const & get_buffer() const {return buf_;}
	private:
		std::vector<uint8_t> buf_;
	};
}
