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
	
	template <typename T> struct vec2_t;
	template <typename T> struct scale2_t;
	template <typename T> struct trans2_t;
	
	template <typename T> struct vec3_t;
	template <typename T> struct euler3_t;
	template <typename T> struct scale3_t;
	template <typename T> struct trans3_t;
	
	template <typename T> struct quaternion_t;
	template <typename T> struct versor_t;
	
	template <typename T> struct mat4_t;
	
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
	
	template <typename T> struct scale2_t : public vec2_t<T> {
		
		using vec2_t<T>::vec2_t;
		inline scale2_t(vec2_t<T> const & v) : vec2_t<T>::data(v.data) {}
		inline scale2_t(T const & v) : vec2_t<T>(v, v) {}
		
		using vec2_t<T>::operator =;
		using vec2_t<T>::operator ==;
		using vec2_t<T>::operator +;
		using vec2_t<T>::operator -;
		using vec2_t<T>::operator *;
		using vec2_t<T>::operator /;
		using vec2_t<T>::operator +=;
		using vec2_t<T>::operator -=;
		using vec2_t<T>::operator *=;
		using vec2_t<T>::operator /=;
		using vec2_t<T>::operator [];
	};
	
	template <typename T> struct trans2_t : public vec2_t<T> {
		
		using vec2_t<T>::vec2_t;
		inline trans2_t(vec2_t<T> const & v) : vec2_t<T>::data(v.data) {}
		
		using vec2_t<T>::operator =;
		using vec2_t<T>::operator ==;
		using vec2_t<T>::operator +;
		using vec2_t<T>::operator -;
		using vec2_t<T>::operator *;
		using vec2_t<T>::operator /;
		using vec2_t<T>::operator +=;
		using vec2_t<T>::operator -=;
		using vec2_t<T>::operator *=;
		using vec2_t<T>::operator /=;
		using vec2_t<T>::operator [];
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
		inline vec3_t<T> operator * (quaternion_t<T> const & quat) {
			vec3_t<T> qw {quat.x, quat.y, quat.z};
			vec3_t<T> t1, t2, t3;
			t1 = cross(*this, qw);
			t1.scale(2);
			t2 = t1.scaled(quat.w);
			t3 = cross(t1, qw);
			t1 = *this;
			t1 += t2;
			t1 += t3;
			return t1;
		}
		inline vec3_t<T> operator / (T const & value) const { return { x / value, y / value, z / value }; }
		
		inline vec3_t<T> & operator += (vec3_t<T> const & other) { x += other.x; y += other.y; z += other.z; return *this; }
		inline vec3_t<T> & operator += (T const & value) { x += value; y += value; z += value; return *this; }
		inline vec3_t<T> & operator -= (vec3_t<T> const & other) { x -= other.x; y -= other.y; z -= other.z; return *this; }
		inline vec3_t<T> & operator -= (T const & value) { x -= value; y -= value; z -= value; return *this; }
		inline vec3_t<T> & operator *= (T const & value) { x *= value; y *= value; z *= value; return *this; }
		inline vec3_t<T> & operator *= (quaternion_t<T> const & quat) {
			vec3_t<T> qw {quat.x, quat.y, quat.z};
			vec3_t<T> t1, t2, t3;
			t1 = cross(*this, qw);
			t1.scale(2);
			t2 = t1.scaled(quat.w);
			t3 = cross(t1, qw);
			operator += (t2);
			operator += (t3);
			return *this;
		}
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
	
	template <typename T> struct quaternion_t {
		T data [4] {0, 0, 0, 0};
		T & x { data[0] };
		T & y { data[1] };
		T & z { data[2] };
		T & w { data[3] };
		
		quaternion_t() = default;
		quaternion_t(T const & x, T const & y, T const & z, T const & w) : data{x, y, z, w} {}
		quaternion_t(vec3_t<T> const & axis, T const & angle) {
			T a = angle/2;
			T s = sin(a);
			data[0] = axis[0] * s;
			data[1] = axis[1] * s;
			data[2] = axis[2] * s;
			data[3] = cos(a);
		}
		
		inline quaternion_t<T> conjugate() {
			return {-x, -y, -z, w};
		}
		
		versor_t<T> versor() {
			return versor_t<T> {*this};
		}
		
		static quaternion_t<T> multiply(quaternion_t<T> const & A, quaternion_t<T> const & B) {
			return quaternion_t<T> {
				(A.data[3] * B.data[0] + A.data[0] * B.data[3] + A.data[1] * B.data[2] - A.data[2] * B.data[1]),
				(A.data[3] * B.data[1] - A.data[0] * B.data[2] + A.data[1] * B.data[3] + A.data[2] * B.data[0]),
				(A.data[3] * B.data[2] + A.data[0] * B.data[1] - A.data[1] * B.data[0] + A.data[2] * B.data[3]),
				(A.data[3] * B.data[3] - A.data[0] * B.data[0] - A.data[1] * B.data[1] - A.data[2] * B.data[2])
			};
		}
		
		inline quaternion_t<T> & operator = (quaternion_t<T> const & other) = default;
		inline quaternion_t<T> & operator = (quaternion_t<T> && other) = default;
		
		inline quaternion_t<T> operator * (quaternion_t<T> const & other) const {
			return quaternion_t<T>::multiply(*this, other);
		}
		inline quaternion_t<T> & operator *= (quaternion_t<T> const & other) {
			*this = quaternion_t<T>::multiply(*this, other);
			return *this;
		}
		
		inline T & operator [] (size_t i) { return data[i]; }
		inline T const & operator [] (size_t i) const { return data[i]; }
		
		template <typename U> quaternion_t<T> & operator = (quaternion_t<U> const & other) {
			data[0] = other.data[0];
			data[1] = other.data[1];
			data[2] = other.data[2];
			data[3] = other.data[3];
			return *this;
		}
	};
	
	template <typename T> struct versor_t : public quaternion_t<T> {
		versor_t(quaternion_t<T> const & q) {
			T v = q.data[0]*q.data[0] + q.data[1]*q.data[1] + q.data[2]*q.data[2] + q.data[3]*q.data[3];
			v = sqrt(v);
			quaternion_t<T>::data[0] = q.data[0] / v;
			quaternion_t<T>::data[1] = q.data[1] / v;
			quaternion_t<T>::data[2] = q.data[2] / v;
			quaternion_t<T>::data[3] = q.data[3] / v;
		}
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
		mat4_t(versor_t<T> const & v) {
			T sqx = v.data[0] * v.data[0];
			T sqy = v.data[1] * v.data[1];
			T sqz = v.data[2] * v.data[2];
			T sqw = v.data[3] * v.data[3];
			data[0][0] =  sqx - sqy - sqz + sqw;
			data[1][1] = -sqx + sqy - sqz + sqw;
			data[2][2] = -sqx - sqy + sqz + sqw;
			T t1 = v.data[0] * v.data[1];
			T t2 = v.data[2] * v.data[3];
			data[1][0] = ((T)2) * (t1 + t2);
			data[0][1] = ((T)2) * (t1 - t2);
			t1 = v.data[0] * v.data[2];
			t2 = v.data[1] * v.data[3];
			data[2][0] = ((T)2) * (t1 - t2);
			data[0][2] = ((T)2) * (t1 + t2);
			t1 = v.data[1] * v.data[2];
			t2 = v.data[0] * v.data[3];
			data[2][1] = ((T)2) * (t1 + t2);
			data[1][2] = ((T)2) * (t1 - t2);
		}
		inline mat4_t(quaternion_t<T> const & q) : mat4_t(q.versor()) {}
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
		
		mat4_t<T> & scale(scale3_t<T> const & v) {
			data[0][0] *= v.x;
			data[1][1] *= v.y;
			data[2][2] *= v.z;
			return *this;
		}
		
		mat4_t<T> scaled(scale3_t<T> const & v) const {
			mat4_t<T> r {*this};
			r.scale(v);
			return r;
		}
		
		mat4_t<T> & translate(trans3_t<T> const & v) {
			data[3][0] *= v.x;
			data[3][1] *= v.y;
			data[3][2] *= v.z;
			return *this;
		}
		
		mat4_t<T> translated(trans3_t<T> const & v) const {
			mat4_t<T> r {*this};
			r.translate(v);
			return r;
		}
		
		T determinant() const {
			return
				data[0][3] * data[1][2] * data[2][1] * data[3][0] - data[0][2] * data[1][3] * data[2][1] * data[3][0] - data[0][3] * data[1][1] * data[2][2] * data[3][0] + data[0][1] * data[1][3] * data[2][2] * data[3][0] +
				data[0][2] * data[1][1] * data[2][3] * data[3][0] - data[0][1] * data[1][2] * data[2][3] * data[3][0] - data[0][3] * data[1][2] * data[2][0] * data[3][1] + data[0][2] * data[1][3] * data[2][0] * data[3][1] +
				data[0][3] * data[1][0] * data[2][2] * data[3][1] - data[0][0] * data[1][3] * data[2][2] * data[3][1] - data[0][2] * data[1][0] * data[2][3] * data[3][1] + data[0][0] * data[1][2] * data[2][3] * data[3][1] +
				data[0][3] * data[1][1] * data[2][0] * data[3][2] - data[0][1] * data[1][3] * data[2][0] * data[3][2] - data[0][3] * data[1][0] * data[2][1] * data[3][2] + data[0][0] * data[1][3] * data[2][1] * data[3][2] +
				data[0][1] * data[1][0] * data[2][3] * data[3][2] - data[0][0] * data[1][1] * data[2][3] * data[3][2] - data[0][2] * data[1][1] * data[2][0] * data[3][3] + data[0][1] * data[1][2] * data[2][0] * data[3][3] +
				data[0][2] * data[1][0] * data[2][1] * data[3][3] - data[0][0] * data[1][2] * data[2][1] * data[3][3] - data[0][1] * data[1][0] * data[2][2] * data[3][3] + data[0][0] * data[1][1] * data[2][2] * data[3][3];
		}
		
		mat4_t<T> inverted() const {
			mat4_t<T> mat {*this};
			mat.data[0][0] = data[1][2] * data[2][3] * data[3][1] - data[1][3] * data[2][2] * data[3][1] + data[1][3] * data[2][1] * data[3][2] - data[1][1] * data[2][3] * data[3][2] - data[1][2] * data[2][1] * data[3][3] + data[1][1] * data[2][2] * data[3][3];
			mat.data[0][1] = data[0][3] * data[2][2] * data[3][1] - data[0][2] * data[2][3] * data[3][1] - data[0][3] * data[2][1] * data[3][2] + data[0][1] * data[2][3] * data[3][2] + data[0][2] * data[2][1] * data[3][3] - data[0][1] * data[2][2] * data[3][3];
			mat.data[0][2] = data[0][2] * data[1][3] * data[3][1] - data[0][3] * data[1][2] * data[3][1] + data[0][3] * data[1][1] * data[3][2] - data[0][1] * data[1][3] * data[3][2] - data[0][2] * data[1][1] * data[3][3] + data[0][1] * data[1][2] * data[3][3];
			mat.data[0][3] = data[0][3] * data[1][2] * data[2][1] - data[0][2] * data[1][3] * data[2][1] - data[0][3] * data[1][1] * data[2][2] + data[0][1] * data[1][3] * data[2][2] + data[0][2] * data[1][1] * data[2][3] - data[0][1] * data[1][2] * data[2][3];
			mat.data[1][0] = data[1][3] * data[2][2] * data[3][0] - data[1][2] * data[2][3] * data[3][0] - data[1][3] * data[2][0] * data[3][2] + data[1][0] * data[2][3] * data[3][2] + data[1][2] * data[2][0] * data[3][3] - data[1][0] * data[2][2] * data[3][3];
			mat.data[1][1] = data[0][2] * data[2][3] * data[3][0] - data[0][3] * data[2][2] * data[3][0] + data[0][3] * data[2][0] * data[3][2] - data[0][0] * data[2][3] * data[3][2] - data[0][2] * data[2][0] * data[3][3] + data[0][0] * data[2][2] * data[3][3];
			mat.data[1][2] = data[0][3] * data[1][2] * data[3][0] - data[0][2] * data[1][3] * data[3][0] - data[0][3] * data[1][0] * data[3][2] + data[0][0] * data[1][3] * data[3][2] + data[0][2] * data[1][0] * data[3][3] - data[0][0] * data[1][2] * data[3][3];
			mat.data[1][3] = data[0][2] * data[1][3] * data[2][0] - data[0][3] * data[1][2] * data[2][0] + data[0][3] * data[1][0] * data[2][2] - data[0][0] * data[1][3] * data[2][2] - data[0][2] * data[1][0] * data[2][3] + data[0][0] * data[1][2] * data[2][3];
			mat.data[2][0] = data[1][1] * data[2][3] * data[3][0] - data[1][3] * data[2][1] * data[3][0] + data[1][3] * data[2][0] * data[3][1] - data[1][0] * data[2][3] * data[3][1] - data[1][1] * data[2][0] * data[3][3] + data[1][0] * data[2][1] * data[3][3];
			mat.data[2][1] = data[0][3] * data[2][1] * data[3][0] - data[0][1] * data[2][3] * data[3][0] - data[0][3] * data[2][0] * data[3][1] + data[0][0] * data[2][3] * data[3][1] + data[0][1] * data[2][0] * data[3][3] - data[0][0] * data[2][1] * data[3][3];
			mat.data[2][2] = data[0][1] * data[1][3] * data[3][0] - data[0][3] * data[1][1] * data[3][0] + data[0][3] * data[1][0] * data[3][1] - data[0][0] * data[1][3] * data[3][1] - data[0][1] * data[1][0] * data[3][3] + data[0][0] * data[1][1] * data[3][3];
			mat.data[2][3] = data[0][3] * data[1][1] * data[2][0] - data[0][1] * data[1][3] * data[2][0] - data[0][3] * data[1][0] * data[2][1] + data[0][0] * data[1][3] * data[2][1] + data[0][1] * data[1][0] * data[2][3] - data[0][0] * data[1][1] * data[2][3];
			mat.data[3][0] = data[1][2] * data[2][1] * data[3][0] - data[1][1] * data[2][2] * data[3][0] - data[1][2] * data[2][0] * data[3][1] + data[1][0] * data[2][2] * data[3][1] + data[1][1] * data[2][0] * data[3][2] - data[1][0] * data[2][1] * data[3][2];
			mat.data[3][1] = data[0][1] * data[2][2] * data[3][0] - data[0][2] * data[2][1] * data[3][0] + data[0][2] * data[2][0] * data[3][1] - data[0][0] * data[2][2] * data[3][1] - data[0][1] * data[2][0] * data[3][2] + data[0][0] * data[2][1] * data[3][2];
			mat.data[3][2] = data[0][2] * data[1][1] * data[3][0] - data[0][1] * data[1][2] * data[3][0] - data[0][2] * data[1][0] * data[3][1] + data[0][0] * data[1][2] * data[3][1] + data[0][1] * data[1][0] * data[3][2] - data[0][0] * data[1][1] * data[3][2];
			mat.data[3][3] = data[0][1] * data[1][2] * data[2][0] - data[0][2] * data[1][1] * data[2][0] + data[0][2] * data[1][0] * data[2][1] - data[0][0] * data[1][2] * data[2][1] - data[0][1] * data[1][0] * data[2][2] + data[0][0] * data[1][1] * data[2][2];
			mat.scale(1 / mat.determinant());
			return mat;
		}
		
		static mat4_t<T> multiply(mat4_t<T> const & A, mat4_t<T> const & B) {
			mat4_t<T> mat;
			mat.data[0][0] = A.data[0][0] * B.data[0][0] + A.data[0][1] * B.data[1][0] + A.data[0][2] * B.data[2][0] + A.data[0][3] * B.data[3][0];
			mat.data[0][1] = A.data[0][0] * B.data[0][1] + A.data[0][1] * B.data[1][1] + A.data[0][2] * B.data[2][1] + A.data[0][3] * B.data[3][1];
			mat.data[0][2] = A.data[0][0] * B.data[0][2] + A.data[0][1] * B.data[1][2] + A.data[0][2] * B.data[2][2] + A.data[0][3] * B.data[3][2];
			mat.data[0][3] = A.data[0][0] * B.data[0][3] + A.data[0][1] * B.data[1][3] + A.data[0][2] * B.data[2][3] + A.data[0][3] * B.data[3][3];
			mat.data[1][0] = A.data[1][0] * B.data[0][0] + A.data[1][1] * B.data[1][0] + A.data[1][2] * B.data[2][0] + A.data[1][3] * B.data[3][0];
			mat.data[1][1] = A.data[1][0] * B.data[0][1] + A.data[1][1] * B.data[1][1] + A.data[1][2] * B.data[2][1] + A.data[1][3] * B.data[3][1];
			mat.data[1][2] = A.data[1][0] * B.data[0][2] + A.data[1][1] * B.data[1][2] + A.data[1][2] * B.data[2][2] + A.data[1][3] * B.data[3][2];
			mat.data[1][3] = A.data[1][0] * B.data[0][3] + A.data[1][1] * B.data[1][3] + A.data[1][2] * B.data[2][3] + A.data[1][3] * B.data[3][3];
			mat.data[2][0] = A.data[2][0] * B.data[0][0] + A.data[2][1] * B.data[1][0] + A.data[2][2] * B.data[2][0] + A.data[2][3] * B.data[3][0];
			mat.data[2][1] = A.data[2][0] * B.data[0][1] + A.data[2][1] * B.data[1][1] + A.data[2][2] * B.data[2][1] + A.data[2][3] * B.data[3][1];
			mat.data[2][2] = A.data[2][0] * B.data[0][2] + A.data[2][1] * B.data[1][2] + A.data[2][2] * B.data[2][2] + A.data[2][3] * B.data[3][2];
			mat.data[2][3] = A.data[2][0] * B.data[0][3] + A.data[2][1] * B.data[1][3] + A.data[2][2] * B.data[2][3] + A.data[2][3] * B.data[3][3];
			mat.data[3][0] = A.data[3][0] * B.data[0][0] + A.data[3][1] * B.data[1][0] + A.data[3][2] * B.data[2][0] + A.data[3][3] * B.data[3][0];
			mat.data[3][1] = A.data[3][0] * B.data[0][1] + A.data[3][1] * B.data[1][1] + A.data[3][2] * B.data[2][1] + A.data[3][3] * B.data[3][1];
			mat.data[3][2] = A.data[3][0] * B.data[0][2] + A.data[3][1] * B.data[1][2] + A.data[3][2] * B.data[2][2] + A.data[3][3] * B.data[3][2];
			mat.data[3][3] = A.data[3][0] * B.data[0][3] + A.data[3][1] * B.data[1][3] + A.data[3][2] * B.data[2][3] + A.data[3][3] * B.data[3][3];
			return mat;
		}
		
		inline mat4_t<T> & operator = (mat4_t<T> const & other) = default;
		inline mat4_t<T> & operator = (mat4_t<T> && other) = default;
		
		inline mat4_t<T> operator * (mat4_t<T> const & other) const {
			return mat4_t<T>::multiply(*this, other);
		}
		
		inline mat4_t<T> & operator *= (mat4_t<T> const & other) {
			*this = mat4_t<T>::multiply(*this, other);
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
