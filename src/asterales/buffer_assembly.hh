#pragma once

#include <cstdint>
#include <cstring>

#include <string>
#include <string_view>

namespace asterales {

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
		
		// ================================
		
		void write(byte_t const * src, size_t size);
		
		inline void write(buffer_assembly const & buf) { write(buf.offset_, buf.size_); }
		
		template <typename T, typename std::enable_if_t<std::is_pod<T>::value && !std::is_pointer<T>::value>* = nullptr>
		inline void write(T const & v, size_t size = sizeof(T)) { write(reinterpret_cast<byte_t const *>(&v), size); }
		
		template <typename T, typename std::enable_if_t<std::is_pod<T>::value && !std::is_pointer<T>::value>* = nullptr>
		inline void write_many(T const * v, size_t count) { write(reinterpret_cast<byte_t const *>(v), count * sizeof(T)); }
		
		// ================================
		
		inline buffer_assembly & operator << (buffer_assembly const & buf) { write(buf); return *this; }
		
		template <typename T, typename std::enable_if_t<std::is_pod<T>::value && !std::is_pointer<T>::value>* = nullptr>
		inline buffer_assembly & operator << (T const & v) { write<T>(v); return *this; }
		
		// ================================
		
		bool precheck(size_t size) const noexcept { return size_ >= size; }
		template <typename T> inline bool precheck() const noexcept { return size_ >= sizeof(T); }
		
		// ================================
		
		void read(byte_t * dest, size_t size);
		
		template <typename T, typename std::enable_if_t<std::is_pod<T>::value && !std::is_pointer<T>::value>* = nullptr>
		inline T read(size_t size = sizeof(T)) { T v {}; read(reinterpret_cast<byte_t *>(&v), size); return v; }
		template <typename T, typename std::enable_if_t<std::is_pod<T>::value && !std::is_pointer<T>::value>* = nullptr>
		inline void read(T & v, size_t size = sizeof(T)) { read(reinterpret_cast<byte_t *>(&v), size); }
		
		template <typename T, typename std::enable_if_t<std::is_pod<T>::value && !std::is_pointer<T>::value>* = nullptr>
		inline void read_many(T * v, size_t count) { read(reinterpret_cast<byte_t *>(v), count * sizeof(T)); }
		
		// ================================
		
		template <typename T, typename std::enable_if_t<std::is_pod<T>::value && !std::is_pointer<T>::value>* = nullptr>
		inline buffer_assembly & operator >> (T & v) { read<T>(v); return *this; }
		
		// ================================
		// string functions
		
		inline void write(std::string const & str) { write(reinterpret_cast<byte_t const *>(str.data()), str.size()); }
		inline void write(std::string_view const & str) { write(reinterpret_cast<byte_t const *>(str.data()), str.size()); }
		inline void write(char const * str) { write(static_cast<std::string_view>(str)); }
		
		inline buffer_assembly & operator << (std::string const & str) { write(str); return *this; }
		inline buffer_assembly & operator << (std::string_view const & str) { write(str); return *this; }
		inline buffer_assembly & operator << (char const *str) { write(str); return *this; }
		
		// ================================
		
		void discard(size_t size);
		
		std::string to_string() const;
		
	private:
		byte_t * data_;
		byte_t * offset_;
		size_t size_, reserve_size_;
	};
	
	struct serializer;
	
	struct serializable {
		virtual ~serializable() = default;
		
		virtual void serialize(serializer &) const = 0;
		virtual void deserialize(serializer &) = 0;
	};
	
	struct serializer {
		
		using byte_t = buffer_assembly::byte_t;
		
		buffer_assembly buffer;
		
		//=================================
		// inherited from buffer_assembly
		
		inline void write(byte_t const * src, size_t size) { buffer.write(src, size); }
		inline void write(buffer_assembly const & buf) { buffer.write(buf); }
		template <typename T, typename std::enable_if_t<std::is_pod<T>::value && !std::is_pointer<T>::value>* = nullptr>
		inline void write(T const & v, size_t size = sizeof(T)) { buffer.write<T>(v, size); }
		template <typename T, typename std::enable_if_t<std::is_pod<T>::value && !std::is_pointer<T>::value>* = nullptr>
		inline void write_many(T const * v, size_t count) { buffer.write_many<T>(v, count); }
		
		inline serializer & operator << (buffer_assembly const & buf) { buffer.write(buf); return *this; }
		template <typename T, typename std::enable_if_t<std::is_pod<T>::value && !std::is_pointer<T>::value>* = nullptr>
		inline serializer & operator << (T const & v) { buffer.write<T>(v); return *this; }
		
		inline void read(byte_t * dest, size_t size) { buffer.write(dest, size); }
		template <typename T, typename std::enable_if_t<std::is_pod<T>::value && !std::is_pointer<T>::value>* = nullptr>
		inline T read(size_t size = sizeof(T)) { return buffer.read<T>(size); }
		template <typename T, typename std::enable_if_t<std::is_pod<T>::value && !std::is_pointer<T>::value>* = nullptr>
		inline void read(T & v, size_t size = sizeof(T)) { buffer.read<T>(v, size); }
		template <typename T, typename std::enable_if_t<std::is_pod<T>::value && !std::is_pointer<T>::value>* = nullptr>
		inline void read_many(T * v, size_t count) { buffer.read_many<T>(v, count); }
		
		template <typename T, typename std::enable_if_t<std::is_pod<T>::value && !std::is_pointer<T>::value>* = nullptr>
		inline serializer & operator >> (T & v) { buffer.read<T>(v); return *this; }
		
		// ================================
		
		inline void write(serializable const & s) { s.serialize(*this); }
		
		template <typename S = size_t, typename T, typename V = typename T::value_type> 
		inline void write_container(T const & cnt) { 
			if constexpr (std::is_pod<V>() && !std::is_pointer<V>()) {
				buffer.write<S>(cnt.size()); 
				buffer.write(reinterpret_cast<byte_t const *>(cnt.data()), cnt.size() * sizeof(V));
			} else if constexpr (!std::is_pod<V>() && std::is_base_of<serializable, V>()) {
				write<S>(cnt.size());
				for (V const & v : cnt) write(v);
			} else {
				static_assert("cannot handle type");
			}
		}
		
		template <typename S = size_t>
		inline void write(std::string const & str) { write_container<S, std::string>(str); }
		template <typename S = size_t>
		inline void write(char const * str) { S len = strlen(str); buffer.write<S>(len); buffer.write(reinterpret_cast<byte_t const *>(str), len); }
		
		// ================================
		
		inline serializer & operator << (serializable const & s) { write(s); return *this; }
		inline serializer & operator << (std::string const & str) { write(str); return *this; }
		inline serializer & operator << (char const * str) { write(str); return *this; }
		
		template <typename T, typename V = typename T::value_type> 
		inline serializer & operator << (T const & cnt) { write_container<size_t, T, V>(cnt); return *this; }
		
		// ================================
		
		inline void read(serializable & s) { s.deserialize(*this); }
		
		template <typename S = size_t, typename T, typename V = typename T::value_type>
		inline void read_container(T & cnt) { 
			if constexpr (std::is_pod<V>() && !std::is_pointer<V>()) {
				cnt.resize(buffer.read<S>());
				buffer.read(reinterpret_cast<byte_t *>(cnt.data()), cnt.size() * sizeof(V));
			} else if constexpr (!std::is_pod<V>() && std::is_base_of<serializable, V>()) {
				S num = buffer.read<S>();
				cnt.clear();
				cnt.reserve(num);
				for (S i = 0; i < num; i++) {
					cnt.emplace_back();
					read(cnt.back());
				}
			} else {
				static_assert("cannot handle type");
			}
		}
		
		template <typename S = size_t, typename T, typename V = typename T::value_type>
		inline T read_container() { T cnt; read_container<S, T, V>(cnt); return cnt; }
		
		template <typename S = size_t> 
		inline std::string read_string() { return read_container<S, std::string>(); }
		template <typename S = size_t> 
		inline void read(std::string & str) { read_container<S, std::string>(str); }
		
		// ================================
		
		inline serializer & operator >> (serializable & s) { s.deserialize(*this); return *this; }
		inline serializer & operator >> (std::string & str) { read(str); return *this; }
		
		template <typename T, typename V = typename T::value_type> 
		inline serializer & operator >> (T & cnt) { read_container<size_t, T, V>(cnt); return *this; }
		
		// ================================
	};
	
}
