#pragma once

#include <cstdint>
#include <cstring>
#include <string>

namespace asterid {
	
	struct buffer_assembly final {
		typedef uint8_t byte_t;
		
		typedef byte_t * iterator;
		typedef byte_t const * const_iterator;
		
		buffer_assembly();
		buffer_assembly(const_iterator begin, const_iterator end);
		buffer_assembly(buffer_assembly const & other);
		buffer_assembly(buffer_assembly && other);
		~buffer_assembly();
		
		buffer_assembly & operator = (buffer_assembly const & other);
		buffer_assembly & operator = (buffer_assembly && other);
		
		bool operator == (buffer_assembly const & other);
		
		inline byte_t * data () { return offset_; }
		inline byte_t const * data () const { return offset_; }
		inline size_t size() const { return size_; }
		inline size_t capacity() const { return reserve_size_; }
		
		void realign();
		void reserve(size_t size);
		void resize(size_t size);
		void shrink(); // shrink to fit
		void clear();
		
		inline byte_t & at(size_t index) noexcept { return offset_[index]; }
		inline byte_t const & at(size_t index) const noexcept { return offset_[index]; }
		inline byte_t & operator [] (size_t index) noexcept { return offset_[index]; }
		inline byte_t const & operator [] (size_t index) const noexcept { return offset_[index]; }
		
		inline iterator begin() noexcept { return offset_; }
		inline iterator end() noexcept { return offset_ + size_; }
		inline const_iterator begin() const noexcept { return offset_; }
		inline const_iterator end() const noexcept { return offset_ + size_; }
		inline const_iterator cbegin() const noexcept { return offset_; }
		inline const_iterator cend() const noexcept { return offset_ + size_; }
		
		std::string hex () const;
		std::string hexlow () const;
		
		inline size_t transfer_to(buffer_assembly & other, size_t num = SIZE_MAX) {
			if (num > size_) num = size_;
			if (num) {
				other.write(offset_, num);
				discard(num);
			}
			return num;
		}
		
		void write(byte_t const * src, size_t size);
		inline void write(char const * src, size_t size) { write(reinterpret_cast<byte_t const *>(src), size); }
		inline void write(char const * src) { write(reinterpret_cast<byte_t const *>(src), strlen(src)); }
		template <typename T, typename = std::enable_if_t<std::is_pod<T>::value && !std::is_pointer<T>::value>> inline void write(T const & v, size_t size = sizeof(T)) { write(reinterpret_cast<byte_t const *>(&v), size); }
		template <typename T, typename = std::enable_if_t<!std::is_pod<T>::value>> inline void write_unsafe(T const & v, size_t size = sizeof(T)) { write(reinterpret_cast<byte_t const *>(&v), size); }
		
		inline buffer_assembly & operator << (buffer_assembly const & other) { write(other.offset_, other.size_); return *this; }
		inline buffer_assembly & operator << (char const * str) { write(str); return *this; }
		inline buffer_assembly & operator << (std::string const & str) { write(reinterpret_cast<byte_t const *>(str.data()), str.size()); return *this; }
		template <typename T, typename = std::enable_if_t<std::is_pod<T>::value && !std::is_pointer<T>::value>> inline buffer_assembly & operator << (T const & v) { write(reinterpret_cast<byte_t const *>(&v), sizeof(T)); return *this; }
		
		bool precheck(size_t size) const noexcept { return size_ >= size; }
		template <typename T> inline bool precheck() const noexcept { return size_ >= sizeof(T); }
		
		void read(byte_t * dest, size_t size);
		inline void read(char * src, size_t size) { read(reinterpret_cast<byte_t *>(src), size); }
		template <typename T, typename = std::enable_if_t<std::is_pod<T>::value && !std::is_pointer<T>::value>> inline T read(size_t size = sizeof(T)) { T v {}; read(reinterpret_cast<byte_t *>(&v), size); return v; }
		template <typename T, typename = std::enable_if_t<!std::is_pod<T>::value>> inline T read_unsafe(size_t size = sizeof(T)) { T v {}; read(reinterpret_cast<byte_t *>(&v), size); return v; }
		
		void discard(size_t size);
		
		std::string to_string() const;
		
	private:
		byte_t * data_;
		byte_t * offset_;
		size_t size_, reserve_size_;
	};
	
}
