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
		byte_buffer(std::string const & str) { buf_.resize(str.size()); memcpy(buf_.data(), str.data(), str.size()); }
		~byte_buffer() = default;

		std::string hex () const;
		std::string hexlow () const;
		inline size_t size () const { return buf_.size(); }
		inline uint8_t * data () { return buf_.data(); }
		inline uint8_t const * data () const { return buf_.data(); }
		inline void reserve (size_t s) {buf_.reserve(s); }
		inline void resize (size_t s) {buf_.resize(s); }
		inline iterator begin () { return buf_.begin(); }
		inline iterator end () { return buf_.end(); }
		inline const_iterator begin () const { return buf_.begin(); }
		inline const_iterator end () const { return buf_.end(); }
		inline iterator insert(const_iterator P, const_iterator A, const_iterator B) { return buf_.insert(P, A, B); }
		inline byte_buffer & operator = (byte_buffer const & other) = default;
		inline byte_buffer & operator = (byte_buffer && other) = default;
		inline byte_buffer operator + (byte_buffer const & other) const { byte_buffer bb {*this}; bb.insert(bb.end(), other.begin(), other.end()); return bb; }
		inline std::vector<uint8_t> & get_buffer() {return buf_;}
		inline std::vector<uint8_t> const & get_buffer() const {return buf_;}
	private:
		std::vector<uint8_t> buf_;
	};
	
}
