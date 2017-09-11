#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>

namespace asterid::json {
	
	struct object;
	
	typedef double real_t;
	typedef int_fast64_t int_t;
	typedef std::string str_t;
	typedef std::vector<object> ary_t;
	typedef std::unordered_map<str_t, object> map_t;
	
	struct object {
		enum struct type : uint_fast8_t {
			none,
			boolean,
			integer,
			real,
			string,
			array,
			map
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

		object & operator [] (size_t);
		object const & operator [] (size_t) const;
		object & operator [] (str_t const &);
		object const & operator [] (str_t const &) const;
		inline object & operator [] (char const * str) { return operator [] (std::string{str}); }
		inline object const & operator [] (char const * str) const { return operator [] (std::string{str}); }
		
		std::string serialize() const;
		
		inline bool is_null() const { return t_ == type::none; }
		inline operator bool () const { return is_null(); }
		
	private:
		type t_ = type::none;
		union {
			bool boolean;
			real_t num_real;
			int_t num_int;
			str_t * str;
			ary_t * ary;
			map_t * map;
		} data;
	};
	
	extern object const & null;
};

std::ostream & operator << (std::ostream & out, asterid::json::object const & t);

namespace std {
	inline string to_string(asterid::json::object const & json) {
		return json.serialize();
	}
}
