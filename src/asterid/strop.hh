#pragma once

#include <algorithm>
#include <functional>
#include <string>
#include <vector>
#include <cstdint>
#include <iostream>
#include <sstream>
#include <iomanip>

namespace asterid {
	
	static std::string const empty_str {};
	
	std::string strf( char const * fmt, ... ) noexcept;
	char const * vas( char const * fmt, ... ) noexcept;
	
	struct streamlogger {
		typedef std::function<void(std::string const &)> logger_cb;
		logger_cb cb;
		std::string str;
		inline streamlogger(std::string const & init = "", logger_cb cb = [](std::string const & str){ printf("%s\n", str.c_str()); }) : cb(cb), str(init) {}
		inline ~streamlogger() { cb(str); }
		
		inline streamlogger & operator << ( std::string const & other ) { str += other; return *this; }
		inline streamlogger & operator << ( char const * other ) { str += other; return *this; }
		template <typename T> inline streamlogger & operator << ( T const & other ) { str += std::to_string(other); return *this; }
	};
	
	template <class T> struct _asterid_non_deducible { using type = T; };
	template <class T> using _asterid_non_deducible_t = typename _asterid_non_deducible<T>::type;
	
	struct strop_bounds_exception {};
		
	/*
	template <typename T, typename V = std::vector<T>> V subset(V const & vec, size_t from, ssize_t to) {
		return V { vec.begin() + from, to < 0 ? vec.end() + to : vec.begin() + to };
	}
	
	template <typename T, typename V = std::vector<T>> V subset_cont(V const & vec, size_t from, size_t count) {
		return V { vec.begin() + from, vec.begin() + from + count };
	}
	*/
	
	template <typename T, typename Ti = typename T::const_iterator> T subset(T const & v, ssize_t b, ssize_t e = 0) {
		Ti bi, ei;
		
		if (b == 0) bi = v.begin();
		else if (b > 0) bi = v.begin() + b;
		else if (b < 0) bi = v.end() - b;
		
		if (e == 0) ei = v.end();
		else if (e > 0) ei = bi + e;
		else if (e < 0) ei = v.end() - e;
		
		return T {bi, ei};
	}
	
	template <typename T, typename Ti = typename T::const_iterator> T subset_safe(T const & v, ssize_t b, ssize_t e = 0) {
		Ti bi, ei;
		
		if (b == 0) bi = v.begin();
		else if (b > 0) bi = v.begin() + b;
		else if (b < 0) bi = v.end() - b;
		
		if (e == 0) ei = v.end();
		else if (e > 0) ei = bi + e;
		else if (e < 0) ei = v.end() - e;
		
		if (ei < bi) throw strop_bounds_exception {};
		if (bi < v.begin()) throw strop_bounds_exception {};
		if (ei > v.end()) throw strop_bounds_exception {};
		
		return T {bi, ei};
	}
	
	template <typename T, typename TT = decltype(T::value_type)> void remove_all(T & v, TT const & tv) {
		if (!v.size()) return;
		v.erase(std::remove(v.begin(), v.end(), tv), v.end());
	}
	
	template <typename T> bool itereq (T const & a_b, T const & a_e, T const & b_b, T const & b_e) {
		T a_i, b_i;
		for (a_i = a_b, b_i = b_b; a_i != a_e && b_i != b_e; a_i++, b_i++) {
			if (*a_i != *b_i) return false;
		}
		if ((a_i == a_e) && (b_i == b_e)) return true;
		return false;
	}
	
	// ================================
	// separate on sequence
	// ================================
	
	template <typename T, typename V = std::vector<T>, typename Ti = typename T::const_iterator> V separate (Ti const & a_b, Ti const & a_e, Ti const & b_b, Ti const & b_e, size_t num_sep = SIZE_MAX) {
		V r {};
		if (a_b == a_e || b_b == b_e) return r;
		size_t cur_sep = 0;
		size_t d = std::distance(b_b, b_e);
		Ti i_b = a_b, i_e = a_b + d, i_l = i_b;
		while (i_e != a_e) {
			if (itereq<Ti>(i_b, i_e, b_b, b_e)) {
				if (i_l != i_b) r.push_back( {i_l, i_b} );
				i_b += d;
				i_l = i_b;
				if (++cur_sep == num_sep) goto end;
				for (size_t j = 0; j < d; j++) if (++i_e == a_e) goto end;
				continue;
			}
			i_b++;
			i_e++;
		}
		end:
		if (itereq<Ti>(i_b, i_e, b_b, b_e) && i_l != i_b) {
			r.push_back( {i_l, i_b} );
		} else if (i_l != a_e) {
			r.push_back( {i_l, a_e} );
		}
		return r;
	}
	
	template <typename T, typename V = std::vector<T>, typename Ti = typename T::const_iterator> V separate (T const & a, _asterid_non_deducible_t<T> const & b, size_t num_sep = SIZE_MAX) {
		return separate<T, V, Ti> (a.begin(), a.end(), b.begin(), b.end(), num_sep);
	}
	
	// ================================
	// separate on element
	// ================================
	
	template <typename T, typename V = std::vector<T>, typename TT = typename T::value_type, typename Ti = typename T::const_iterator> V separate (Ti const & a_b, Ti const & a_e, TT const & b_v, size_t num_sep = SIZE_MAX) {
		V r {};
		if (a_b == a_e) return r;
		size_t cur_sep = 0;
		Ti i = a_b, i_l = i;
		while (i != a_e) {
			if (*i == b_v) {
				if (i_l != i) r.push_back( {i_l, i} );
				i_l = ++i;
				if (++cur_sep == num_sep) break;
				if (i == a_e) break;
				continue;
			}
			i++;
		}
		
		if (*i == b_v && i != i_l) {
			r.push_back( {i_l, i} );
		} else if (i_l != a_e) {
			r.push_back( {i_l, a_e} );
		}
		return r;
	}
	
	template <typename T, typename V = std::vector<T>, typename TT = typename T::value_type, typename Ti = typename T::const_iterator> V separate (T const & a, TT const & b_v, size_t num_sep = SIZE_MAX) {
		return separate<T, V, TT, Ti> (a.begin(), a.end(), b_v, num_sep);
	}
	
	// ================================
	// separate on any sequence
	// ================================
	
	/*
	template <typename T, typename V = std::vector<T>, typename Ti = typename T::const_iterator, typename SV = std::vector<T>, typename SVi = typename SV::const_iterator> V separate_any (Ti const & a_b, Ti const & a_e, SVi const & b_b, SVi const & b_e, size_t num_sep = SIZE_MAX) {
		V r {};
		if (a_b == a_e || b_b == b_e) return r;
		size_t cur_sep = 0;
		Ti i_b = a_b, i_l = i_b;
		while (i_b != a_e) {
			for (auto const & seq : b_b, b_e) {
				if (i_l != i_b) r.push_back( {i_l, i_b} );
				i_b += d;
				i_l = i_b;
				if (++cur_sep == num_sep) goto end;
				for (size_t j = 0; j < d; j++) if (++i_e == a_e) goto end;
				continue;
			}
			i_b++;
			i_e++;
		}
		end:
		if (itereq<Ti>(i_b, i_e, b_b, b_e) && i_l != i_b) {
			r.push_back( {i_l, i_b} );
		} else if (i_l != a_e) {
			r.push_back( {i_l, a_e} );
		}
		return r;
	}
	
	template <typename T, typename V = std::vector<T>, typename Ti = typename T::const_iterator, typename SV = std::vector<T>, typename SVi = typename SV::const_iterator> V separate_any (T const & a, SV const & b, size_t num_sep = SIZE_MAX) {
		return separate_any<T, V, Ti, SV, SVi> (a.begin(), a.end(), b.begin(), b.end(), num_sep);
	}
	*/
	
	// ================================
	// separate on any element
	// ================================
	
	template <typename T, typename V = std::vector<T>, typename TT = typename T::value_type, typename Ti = typename T::const_iterator> V separate_any (Ti const & a_b, Ti const & a_e, Ti const & b_b, Ti const & b_e, size_t num_sep = SIZE_MAX) {
		V r {};
		if (a_b == a_e) return r;
		size_t cur_sep = 0;
		Ti i = a_b, i_l = i;
		while (i != a_e) {
			if (std::any_of(b_b, b_e, [&i](TT const & v){return *i == v;})) {
				if (i_l != i) r.push_back( {i_l, i} );
				i_l = ++i;
				if (++cur_sep == num_sep) break;
				if (i == a_e) break;
				continue;
			}
			i++;
		}
		
		if (std::any_of(b_b, b_e, [&i](TT const & v){return *i == v;}) && i != i_l) {
			r.push_back( {i_l, i} );
		} else if (i_l != a_e) {
			r.push_back( {i_l, a_e} );
		}
		return r;
	}
	
	template <typename T, typename V = std::vector<T>, typename TT = typename T::value_type, typename Ti = typename T::const_iterator> V separate_any (T const & a, T const & b_v, size_t num_sep = SIZE_MAX) {
		return separate_any<T, V, TT, Ti> (a.begin(), a.end(), b_v.begin(), b_v.end(), num_sep);
	}
	
	// ================================
	
	template <typename T, typename TT = decltype(T::value_type)> T replace(T const & c, TT const & v) {
		
	}
	
	template <typename T, typename TT = decltype(T::value_type)> void trim(T & v, TT tv) {
		while (v.size() && *v.begin() == tv) v.erase(v.begin());
		while (v.size() && v.back() == tv) v.erase(v.end() - 1);
	}
	
	template <typename T, typename TT = decltype(T::value_type)> void trimb(T & v, TT tv) {
		while (v.size() && *v.begin() == tv) v.erase(v.begin());
	}
	
	template <typename T, typename TT = decltype(T::value_type)> void trime(T & v, TT tv) {
		while (v.size() && v.back() == tv) v.erase(v.end() - 1);
	}
	
	static inline bool issymbol1(char c) { return c < 48; }
	static inline bool isnum(char c) { return c > 47 && c < 58; }
	
	template <typename T, typename Ti = typename T::const_iterator> bool natcmp (Ti const & a_b, Ti const & a_e, Ti const & b_b, Ti const & b_e) {
		for (Ti a_i = a_b, b_i = b_b; a_i != a_e && b_i != b_e;) {
			
			if (issymbol1(*a_i)) {
				if (issymbol1(*b_i)) {
					if (*a_i < *b_i) return true;
					if (*a_i > *b_i) return false;
				} else return true;
			} else if (issymbol1(*b_i)) {
				return false;
			}
			
			if (isnum(*a_i)) {
				if (isnum(*b_i)) {
					Ti a_ib = a_i;
					Ti b_ib = b_i;
					for(;a_i != a_e && isnum(*a_i); a_i++);
					for(;b_i != b_e && isnum(*b_i); b_i++);
					unsigned long num_a = strtoul(std::string{a_ib, a_i}.c_str(), nullptr, 10);
					unsigned long num_b = strtoul(std::string{b_ib, b_i}.c_str(), nullptr, 10);
					if (num_a < num_b) return true;
					else if (num_a > num_b) return false;
					else continue;
				} else return true;
			} else if (isnum(*b_i)) {
				return false;
			}
			
			if (*a_i < *b_i) return true;
			if (*a_i > *b_i) return false;
			a_i++, b_i++;
			continue;
		}
		return false;
	}
	
	template <typename T> bool starts_with(T const & string_to_search, _asterid_non_deducible_t<T> const & string_to_search_for) {
		if (string_to_search.size() < string_to_search_for.size()) return false;
		typename T::const_iterator a_i = string_to_search.begin(), b_i = string_to_search_for.begin();
		for (size_t i = 0; i < string_to_search_for.size(); i++) {
			if (*a_i++ != *b_i++) return false;
		}
		return true;
	}
	
	template <typename T> bool ends_with(T const & string_to_search, _asterid_non_deducible_t<T> const & string_to_search_for) {
		if (string_to_search.size() < string_to_search_for.size()) return false;
		typename T::const_iterator a_i = string_to_search.end() - string_to_search_for.size(), b_i = string_to_search_for.begin();
		for (size_t i = 0; i < string_to_search_for.size(); i++) {
			if (*a_i++ != *b_i++) return false;
		}
		return true;
	}
	
	inline std::string human_readable_size(size_t bytes) {
		if (bytes < 2048) return std::to_string(bytes) + " B";
		std::stringstream ss; ss << std::fixed << std::showpoint << std::setprecision(2);
		if (bytes < 2097152) {
			ss << bytes / 1024.0 << " KiB";
			return ss.str();
		}
		if (bytes < 2147483648) {
			ss << bytes / 1048576.0 << " MiB";
			return ss.str();
		}
		ss << bytes / 1073741824.0 << " GiB";
		return ss.str();
	}
	
}

#define _as_here asterid::vas("%s, in %s, line %u", __PRETTY_FUNCTION__, __FILE__,  __LINE__)
#define _as_herestr asterid::strf("%s, in %s, line %u", __PRETTY_FUNCTION__, __FILE__,  __LINE__)
