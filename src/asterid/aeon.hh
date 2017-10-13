#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>

#include "buffer_assembly.hh"

namespace asterid::aeon {
	
	struct object;
	
	typedef double real_t;
	typedef int_fast64_t int_t;
	typedef std::string str_t;
	typedef std::vector<object> ary_t;
	typedef std::unordered_map<str_t, object> map_t;
	typedef buffer_assembly bin_t;
	
	namespace exception {
		struct parse {};
	}
	
	struct object {
		enum struct type : uint_fast8_t {
			none,
			boolean,
			integer,
			real,
			string,
			array,
			map,
			binary
		};
		object() = default;
		object(type);
		
		object(bool);
		explicit object(int_t);
		explicit object(real_t);
		object(str_t const &);
		object(str_t &&);
		object(ary_t const &);
		object(ary_t &&);
		object(map_t const &);
		object(map_t &&);
		object(bin_t const &);
		object(bin_t &&);
		
		inline object(char const * str) : object( std::string {str} ) {}
		template <typename T> inline object(T i, typename std::enable_if<std::is_integral<T>::value>::type* = 0) : object(static_cast<int_t>(i)) {}
		template <typename T> inline object(T i, typename std::enable_if<std::is_floating_point<T>::value>::type* = 0) : object(static_cast<real_t>(i)) {}
		
		object(object const &);
		object(object &&);
		template <typename T> inline object & operator = (T const & v) { *this = object {v}; return *this; }
		template <typename T> inline object & operator = (T && v) { *this = object {v}; return *this; }
		object & operator = (object const &);
		object & operator = (object &&);
		
		~object();
		
		bool & boolean();
		bool const & boolean() const;
		int_t & integer();
		int_t const & integer() const;
		real_t & real();
		real_t const & real() const;
		str_t & string();
		str_t const & string() const;
		ary_t & array();
		ary_t const & array() const;
		map_t & map();
		map_t const & map() const;
		bin_t & binary();
		bin_t const & binary() const;
		
		bool as_boolean() const;
		int_t as_integer() const;
		real_t as_real() const;
		str_t as_string() const;
		
		inline operator bool () const { return as_boolean(); }
		inline operator int_t () const { return as_integer(); }
		inline operator real_t () const { return as_real(); }
		inline operator str_t () const { return as_string(); }
		inline operator ary_t const & () const { return array(); }
		inline operator map_t const & () const { return map(); }
		inline operator bin_t const & () const { return binary(); }

		object & operator [] (size_t);
		object const & operator [] (size_t) const;
		object & operator [] (str_t const &);
		object const & operator [] (str_t const &) const;
		inline object & operator [] (char const * str) { return operator [] (std::string{str}); }
		inline object const & operator [] (char const * str) const { return operator [] (std::string{str}); }
		
		inline bool is_null() const { return t_ == type::none; }
		
		inline bool is_bool() const { return t_ == type::boolean; }
		inline bool is_integer() const { return t_ == type::integer; }
		inline bool is_real() const { return t_ == type::real; }
		inline bool is_numerical() const { return t_ == type::integer || t_ == type::real; }
		inline bool is_string() const { return t_ == type::string; }
		inline bool is_array() const { return t_ == type::array; }
		inline bool is_map() const { return t_ == type::map; }
		inline bool is_binary() const { return t_ == type::binary; }
		
		std::string serialize_text() const;
		buffer_assembly serialize_binary() const;
		void serialize_binary(buffer_assembly &) const;
		
		static object parse_text(std::string const &);
		static object parse_binary(buffer_assembly & buf);
		
		bool operator == (object const & other) const;
		
	private:
		type t_ = type::none;
		union {
			bool boolean;
			real_t num_real;
			int_t num_int;
			str_t * str;
			ary_t * ary;
			map_t * map;
			bin_t * bin;
		} data;
	};
	
	inline object parse_text(std::string const & text) { return object::parse_text(text); }
	inline object parse_binary(buffer_assembly & buf) { return object::parse_binary(buf); }
	
	inline object string() { return object::type::string; }
	inline object array() { return object::type::array; }
	inline object map() { return object::type::map; }
	inline object binary() { return object::type::binary; }
	
	extern object const & null;
};

std::ostream & operator << (std::ostream & out, asterid::aeon::object const & t);

namespace std {
	inline string to_string(asterid::aeon::object const & aeon) {
		return aeon.serialize_text();
	}
}
