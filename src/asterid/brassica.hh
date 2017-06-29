#pragma once

#include <ctgmath>

namespace asterid::brassica {
	
	template <typename T, T M = 1> T constexpr pi = M * static_cast<T>(3.141592653589793238462643383279502884197169399375105820974944592307816406286208998628034825342117067982148086513282306647093844609550582231725359408128481117450284102701938521105559644622948954930381964428810975665933446128475648233786783165271201909145649L);
	
	template <typename T> T const & min(T const & A, T const & B) { return A > B ? B : A; }
	template <typename T> T & min(T & A, T & B) { return A > B ? B : A; }
	template <typename T> T const & max(T const & A, T const & B) { return A > B ? A : B; }
	template <typename T> T & max(T & A, T & B) { return A > B ? A : B; }
	template <typename T> void clamp(T & V, T const & MIN, T const & MAX) { if (V > MAX) V = MAX; else if (V < MIN) V = MIN; }
	template <typename T> T clamped(T const & V, T const & MIN, T const & MAX) { if (V > MAX) return MAX; else if (V < MIN) return MIN; else return V; }
	
//================================================================================================
//------------------------------------------------------------------------------------------------
//================================================================================================
	
	template <typename T> struct vec2_t {
		
		typedef T length_t;
		T data [2] {0, 0};
		T & x { data[0] };
		T & y { data[1] };
		
		vec2_t() = default;
		vec2_t(T x, T y) : data {x, y} {}
		vec2_t(vec2_t const &) = default;
		vec2_t(vec2_t &&) = default;
		template <typename U> vec2_t(vec2_t<U> const & other) { x = other.x; y = other.y; }
		
		static inline T dot(vec2_t<T> const & A, vec2_t<T> const & B) {
			return A.x * B.x + A.y * B.y;
		}
		
		inline T magnitude() const { return static_cast<T>(sqrt((x*x)+(y*y))); }
		
		inline T & normalize() {
			T mag = magnitude();
			x /= mag;
			y /= mag;
			return *this;
		}
		
		inline T normalized() {
			T mag = magnitude();
			return { x/mag, y/mag };
		}
		
		inline vec2_t<T> & operator = (vec2_t<T> const & other) { x = other.x; y = other.y; return *this; }
		inline bool operator == (vec2_t<T> const & other) const { return x == other.x && y == other.y; }
		inline vec2_t<T> operator + (vec2_t<T> const & other) const { return {x + other.x, y + other.y}; }
		inline vec2_t<T> operator + (T const & value) const { return {x + value, y + value}; }
		inline vec2_t<T> operator - (vec2_t<T> const & other) const { return {x - other.x, y - other.y}; }
		inline vec2_t<T> operator - (T const & value) const { return {x - value, y - value}; }
		inline T operator * (vec2_t<T> const & other) const { return dot(*this, other); }
		inline vec2_t<T> operator * (T const & value) const { return { x * value, y * value }; }
		inline vec2_t<T> operator / (T const & value) const { return { x / value, y / value }; }
		
		inline vec2_t<T> & operator += (vec2_t<T> const & other) { x += other.x; y += other.y; return *this; }
		inline vec2_t<T> & operator += (T const & value) { x += value; y += value; return *this; }
		inline vec2_t<T> & operator -= (vec2_t<T> const & other) { x -= other.x; y -= other.y; return *this; }
		inline vec2_t<T> & operator -= (T const & value) { x -= value; y -= value; return *this; }
		inline vec2_t<T> & operator *= (T const & value) { x *= value; y *= value; return *this; }
		inline vec2_t<T> & operator /= (T const & value) { x /= value; y /= value; return *this; }
		
		template <typename U> vec2_t<T> operator = (vec2_t<U> const & other) { x = other.x; y = other.y; return *this; }
		
		inline T & operator [] (size_t i) { return data[i]; }
		inline T const & operator [] (size_t i) const { return data[i]; }
	};
	
//================================================================================================
//------------------------------------------------------------------------------------------------
//================================================================================================
	
	template <typename T> struct vec3_t {
		
		typedef T length_t;
		T data [3] {0, 0, 0};
		T & x { data[0] };
		T & y { data[1] };
		T & z { data[2] };
		
		vec3_t() = default;
		vec3_t(T x, T y, T z) : data {x, y, z} {}
		vec3_t(vec3_t const &) = default;
		vec3_t(vec3_t &&) = default;
		template <typename U> vec3_t(vec3_t<U> const & other) { x = other.x; y = other.y; z = other.z; }
		
		static inline T dot(vec3_t<T> const & A, vec3_t<T> const & B) {
			return A.x * B.x + A.y * B.y + A.z * B.z;
		}
		
		static vec3_t<T> cross(vec3_t<T> const & A, vec3_t<T> const & B) {
			return {
				A.y * B.z - A.z * B.y,
				A.z * B.x - A.x * B.z,
				A.x * B.y - A.y * B.x,
			};
		}
		
		inline T magnitude() const { return static_cast<T>(sqrt((x*x)+(y*y)+(z*z))); }
		
		inline T & normalize() {
			T mag = magnitude();
			x /= mag;
			y /= mag;
			z /= mag;
			return *this;
		}
		
		inline T normalized() {
			T mag = magnitude();
			return { x/mag, y/mag, z/mag };
		}
		
		inline vec3_t<T> & operator = (vec3_t<T> const & other) { x = other.x; y = other.y; z = other.z; return *this; }
		inline bool operator == (vec3_t<T> const & other) const { return x == other.x && y == other.y && z == other.z; }
		inline vec3_t<T> operator + (vec3_t<T> const & other) const { return { x + other.x, y + other.y, z + other.z }; }
		inline vec3_t<T> operator + (T const & value) const { return { x + value, y + value, z + value }; }
		inline vec3_t<T> operator - (vec3_t<T> const & other) const { return { x - other.x, y - other.y, z - other.z }; }
		inline vec3_t<T> operator - (T const & value) const { return { x - value, y - value, z - value }; }
		inline vec3_t<T> operator * (T const & value) const { return { x * value, y * value, z * value }; }
		inline vec3_t<T> operator / (T const & value) const { return { x / value, y / value, z / value }; }
		
		inline vec3_t<T> & operator += (vec3_t<T> const & other) { x += other.x; y += other.y; z += other.z; return *this; }
		inline vec3_t<T> & operator += (T const & value) { x += value; y += value; z += value; return *this; }
		inline vec3_t<T> & operator -= (vec3_t<T> const & other) { x -= other.x; y -= other.y; z -= other.z; return *this; }
		inline vec3_t<T> & operator -= (T const & value) { x -= value; y -= value; z -= value; return *this; }
		inline vec3_t<T> & operator *= (T const & value) { x *= value; y *= value; z *= value; return *this; }
		inline vec3_t<T> & operator /= (T const & value) { x /= value; y /= value; z /= value; return *this; }
		
		template <typename U> vec3_t<T> & operator = (vec3_t<U> const & other) { x = other.x; y = other.y; z = other.z; return *this; }
		
		inline T & operator [] (size_t i) { return data[i]; }
		inline T const & operator [] (size_t i) const { return data[i]; }
	};
	
	template <typename T> struct scale3_t : public vec3_t<T> {
		
		using vec3_t<T>::vec3_t;
		inline scale3_t(vec3_t<T> const & v) : vec3_t<T>::data(v.data) {}
		inline scale3_t(T const & v) : vec3_t<T>(v, v, v) {}
		
		using vec3_t<T>::operator =;
		using vec3_t<T>::operator ==;
		using vec3_t<T>::operator +;
		using vec3_t<T>::operator -;
		using vec3_t<T>::operator *;
		using vec3_t<T>::operator /;
		using vec3_t<T>::operator +=;
		using vec3_t<T>::operator -=;
		using vec3_t<T>::operator *=;
		using vec3_t<T>::operator /=;
		using vec3_t<T>::operator [];
	};
	
	template <typename T> struct trans3_t : public vec3_t<T> {
		
		using vec3_t<T>::vec3_t;
		inline trans3_t(vec3_t<T> const & v) : vec3_t<T>::data(v.data) {}
		
		using vec3_t<T>::operator =;
		using vec3_t<T>::operator ==;
		using vec3_t<T>::operator +;
		using vec3_t<T>::operator -;
		using vec3_t<T>::operator *;
		using vec3_t<T>::operator /;
		using vec3_t<T>::operator +=;
		using vec3_t<T>::operator -=;
		using vec3_t<T>::operator *=;
		using vec3_t<T>::operator /=;
		using vec3_t<T>::operator [];
	};
	
	template <typename T> struct euler3_t : public vec3_t<T> {
		
		T & roll { vec3_t<T>::data[0] };
		T & pitch { vec3_t<T>::data[1] };
		T & yaw { vec3_t<T>::data[2] };
		
		using vec3_t<T>::vec3_t;
		inline euler3_t(vec3_t<T> const & v) : vec3_t<T>::data(v.data) {}
		
		using vec3_t<T>::operator =;
		using vec3_t<T>::operator ==;
		using vec3_t<T>::operator +;
		using vec3_t<T>::operator -;
		using vec3_t<T>::operator *;
		using vec3_t<T>::operator /;
		using vec3_t<T>::operator +=;
		using vec3_t<T>::operator -=;
		using vec3_t<T>::operator *=;
		using vec3_t<T>::operator /=;
		using vec3_t<T>::operator [];
	};
	
//================================================================================================
//------------------------------------------------------------------------------------------------
//================================================================================================
	
	template <typename T> struct quaternion {
		T data [4];
	};
	
//================================================================================================
//------------------------------------------------------------------------------------------------
//================================================================================================
	
	template <typename T> struct mat4_t {
		T data [4][4] {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};
		
		mat4_t() = default;
		mat4_t(scale3_t<T> const & v) {
			data[0][0] = v.x;
			data[1][1] = v.y;
			data[2][2] = v.z;
		}
		mat4_t(trans3_t<T> const & v) {
			data[3][0] = v.x;
			data[3][1] = v.y;
			data[3][2] = v.z;
		}
		mat4_t(euler3_t<T> const & v) {
			T rs = sin(-v.roll);
			T ps = sin(v.pitch);
			T ys = sin(-v.yaw);
			T rc = cos(-v.roll);
			T pc = cos(v.pitch);
			T yc = cos(-v.yaw);
			data[0][0] = yc * pc;
			data[0][1] = ps * rs - pc * ys * rc;
			data[0][2] = pc * ys * rs + ps * rc;
			data[0][3] = 0;
			data[1][0] = ys;
			data[1][1] = yc * rc;
			data[1][2] = -yc * rs;
			data[1][3] = 0;
			data[2][0] = -ps * yc;
			data[2][1] = ps * ys * rc + pc * rs;
			data[2][2] = -ps * yc * rs + pc * rc;
			data[3][0] = 0;
			data[3][1] = 0;
			data[3][2] = 0;
			data[3][3] = 1;
		}
		mat4_t(mat4_t const &) = default;
		mat4_t(mat4_t &&) = default;
		template <typename U> mat4_t(mat4_t<U> const & other) {
			data[0][0] = other.data[0][0];
			data[0][1] = other.data[0][1];
			data[0][2] = other.data[0][2];
			data[0][3] = other.data[0][3];
			data[1][0] = other.data[1][0];
			data[1][1] = other.data[1][1];
			data[1][2] = other.data[1][2];
			data[1][3] = other.data[1][3];
			data[2][0] = other.data[2][0];
			data[2][1] = other.data[2][1];
			data[2][2] = other.data[2][2];
			data[2][3] = other.data[2][3];
			data[3][0] = other.data[3][0];
			data[3][1] = other.data[3][1];
			data[3][2] = other.data[3][2];
			data[3][3] = other.data[3][3];
		}
		
		mat4_t<T> & scale(T const & v) {
			data[0][0] *= v;
			data[1][1] *= v;
			data[2][2] *= v;
			return *this;
		}
		
		mat4_t<T> & scale(vec3_t<T> const & v) {
			data[0][0] *= v.x;
			data[1][1] *= v.y;
			data[2][2] *= v.z;
			return *this;
		}
		
		mat4_t<T> scaled(T const & v) const {
			mat4_t<T> r {*this};
			r.scale(v);
			return r;
		}
		
		mat4_t<T> scaled(vec3_t<T> const & v) const {
			mat4_t<T> r {*this};
			r.scale(v);
			return r;
		}
		
		mat4_t<T> & translate(vec3_t<T> const & v) {
			data[3][0] *= v.x;
			data[3][1] *= v.y;
			data[3][2] *= v.z;
			return *this;
		}
		
		mat4_t<T> translated(vec3_t<T> const & v) const {
			mat4_t<T> r {*this};
			r.translate(v);
			return r;
		}
		
		inline mat4_t<T> & operator = (mat4_t<T> const & other) {
			data[0][0] = other.data[0][0];
			data[0][1] = other.data[0][1];
			data[0][2] = other.data[0][2];
			data[0][3] = other.data[0][3];
			data[1][0] = other.data[1][0];
			data[1][1] = other.data[1][1];
			data[1][2] = other.data[1][2];
			data[1][3] = other.data[1][3];
			data[2][0] = other.data[2][0];
			data[2][1] = other.data[2][1];
			data[2][2] = other.data[2][2];
			data[2][3] = other.data[2][3];
			data[3][0] = other.data[3][0];
			data[3][1] = other.data[3][1];
			data[3][2] = other.data[3][2];
			data[3][3] = other.data[3][3];
			return *this;
		}
		
		template <typename U> mat4_t<T> & operator = (mat4_t<U> const & other) {
			data[0][0] = other.data[0][0];
			data[0][1] = other.data[0][1];
			data[0][2] = other.data[0][2];
			data[0][3] = other.data[0][3];
			data[1][0] = other.data[1][0];
			data[1][1] = other.data[1][1];
			data[1][2] = other.data[1][2];
			data[1][3] = other.data[1][3];
			data[2][0] = other.data[2][0];
			data[2][1] = other.data[2][1];
			data[2][2] = other.data[2][2];
			data[2][3] = other.data[2][3];
			data[3][0] = other.data[3][0];
			data[3][1] = other.data[3][1];
			data[3][2] = other.data[3][2];
			data[3][3] = other.data[3][3];
			return *this;
		}
		
		inline T & operator [] (size_t i) { return data[i]; }
		inline T const & operator [] (size_t i) const { return data[i]; }
	};
	
}
