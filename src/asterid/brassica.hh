#pragma once

#ifndef BRASSICA_PRINT_FUNCTIONS
#define BRASSICA_PRINT_FUNCTIONS 1
#endif

#include <ctgmath>

#if BRASSICA_PRINT_FUNCTIONS == 1
#include <string>
#endif

namespace asterid::brassica {
	
	template <typename T> T constexpr pi = static_cast<T>(3.141592653589793238462643383279502884197169399375105820974944592307816406286208998628034825342117067982148086513282306647093844609550582231725359408128481117450284102701938521105559644622948954930381964428810975665933446128475648233786783165271201909145649L);
	template <typename T> T deg2rad(T const & v) { return v * pi<T> / static_cast<T>(180); }
	template <typename T> T rad2deg(T const & v) { return v / pi<T> * static_cast<T>(180); }
	
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
	template <typename T> struct vec3_t;
	
	template <typename T> struct quaternion_t;
	template <typename T> struct versor_t;
	
	template <typename T> struct mat4_t;
	
//================================================================================================
//------------------------------------------------------------------------------------------------
//================================================================================================
	
	template <typename T> struct vec2_t {
		
		typedef T length_t;
		T data [2] {0, 0};
		
		inline T & x() { return data[0]; }
		inline T & y() { return data[1]; }
		inline T & width() { return data[0]; }
		inline T & height() { return data[1]; }
		
		vec2_t() = default;
		vec2_t(T x, T y) : data {x, y} {}
		vec2_t(vec2_t const &) = default;
		vec2_t(vec2_t &&) = default;
		template <typename U> vec2_t(vec2_t<U> const & other) { data[0] = other.data[0]; data[1] = other.data[1]; }
		
		static inline T dot(vec2_t<T> const & A, vec2_t<T> const & B) {
			return A.data[0] * B.data[0] + A.data[1] * B.data[1];
		}
		
		inline T magnitude() const { return static_cast<T>(sqrt( (data[0] * data[0]) + (data[1] * data[1]) )); }
		
		inline T & normalize() {
			T mag = magnitude();
			data[0] /= mag;
			data[1] /= mag;
			return *this;
		}
		
		inline T normalized() {
			T mag = magnitude();
			return { data[0]/mag, data[1]/mag };
		}
		
		inline vec2_t<T> & operator = (vec2_t<T> const & other) = default;
		
		inline bool operator == (vec2_t<T> const & other) const { return data[0] == other.data[0] && data[1] == other.data[1]; }
		
		inline vec2_t<T> operator + (vec2_t<T> const & other) const { return {data[0] + other.data[0], data[1] + other.data[1]}; }
		inline vec2_t<T> operator + (T const & value) const { return {data[0] + value, data[1] + value}; }
		inline vec2_t<T> operator - (vec2_t<T> const & other) const { return {data[0] - other.data[0], data[1] - other.data[1]}; }
		inline vec2_t<T> operator - (T const & value) const { return {data[0] - value, data[1] - value}; }
		inline T operator * (vec2_t<T> const & other) const { return dot(*this, other); }
		inline vec2_t<T> operator * (T const & value) const { return { data[0] * value, data[1] * value }; }
		inline vec2_t<T> operator / (T const & value) const { return { data[0] / value, data[1] / value }; }
		
		inline vec2_t<T> & operator += (vec2_t<T> const & other) { data[0] += other.data[0]; data[1] += other.data[1]; return *this; }
		inline vec2_t<T> & operator += (T const & value) { data[0] += value; data[1] += value; return *this; }
		inline vec2_t<T> & operator -= (vec2_t<T> const & other) { data[0] -= other.data[0]; data[1] -= other.data[1]; return *this; }
		inline vec2_t<T> & operator -= (T const & value) { data[0] -= value; data[1] -= value; return *this; }
		inline vec2_t<T> & operator *= (T const & value) { data[0] *= value; data[1] *= value; return *this; }
		inline vec2_t<T> & operator /= (T const & value) { data[0] /= value; data[1] /= value; return *this; }
		
		template <typename U> vec2_t<T> operator = (vec2_t<U> const & other) { data[0] = other.data[0]; data[1] = other.data[1]; return *this; }
		
		inline T & operator [] (size_t i) { return data[i]; }
		inline T const & operator [] (size_t i) const { return data[i]; }
		
#if BRASSICA_PRINT_FUNCTIONS == 1
		std::string to_string() {
			return "V2[" + std::to_string(data[0]) + ", " + std::to_string(data[1]) + "]";
		}
#endif
	};
	
//================================================================================================
//------------------------------------------------------------------------------------------------
//================================================================================================
	
	template <typename T> struct vec3_t {
		
		typedef T length_t;
		T data [3] {0, 0, 0};
		inline T & x() { return data[0]; }
		inline T & y() { return data[1]; }
		inline T & z() { return data[2]; }
		inline T const & x() const { return data[0]; }
		inline T const & y() const { return data[1]; }
		inline T const & z() const { return data[2]; }
		inline T & roll() { return data[0]; }
		inline T & pitch() { return data[1]; }
		inline T & yaw() { return data[2]; }
		inline T const & roll() const { return data[0]; }
		inline T const & pitch() const { return data[1]; }
		inline T const & yaw() const { return data[2]; }
		inline T & r() { return data[0]; }
		inline T & g() { return data[1]; }
		inline T & b() { return data[2]; }
		inline T const & r() const { return data[0]; }
		inline T const & g() const { return data[1]; }
		inline T const & b() const { return data[2]; }
		
		vec3_t() = default;
		vec3_t(T v) : data {v, v, v} {}
		vec3_t(T x, T y, T z) : data {x, y, z} {}
		vec3_t(vec3_t const &) = default;
		vec3_t(vec3_t &&) = default;
		template <typename U> vec3_t(vec3_t<U> const & other) { data[0] = other.data[0]; data[1] = other.data[1]; data[2] = other.data[2]; }
		
		static inline T dot(vec3_t<T> const & A, vec3_t<T> const & B) {
			return A.data[0] * B.data[0] + A.data[1] * B.data[1] + A.data[2] * B.data[2];
		}
		
		static vec3_t<T> cross(vec3_t<T> const & A, vec3_t<T> const & B) {
			return {
				A.data[1] * B.data[2] - A.data[2] * B.data[1],
				A.data[2] * B.data[0] - A.data[0] * B.data[2],
				A.data[0] * B.data[1] - A.data[1] * B.data[0],
			};
		}
		
		inline T magnitude() const { return static_cast<T>(sqrt((data[0]*data[0])+(data[1]*data[1])+(data[2]*data[2]))); }
		
		inline T & normalize() {
			T mag = magnitude();
			data[0] /= mag;
			data[1] /= mag;
			data[2] /= mag;
			return *this;
		}
		
		inline T normalized() {
			T mag = magnitude();
			return { data[0]/mag, data[1]/mag, data[2]/mag };
		}
		
		inline vec3_t<T> & operator = (vec3_t<T> const & other) = default;
		inline vec3_t<T> & operator = (vec3_t<T> && other) = default;
		
		inline bool operator == (vec3_t<T> const & other) const { return data[0] == other.data[0] && data[1] == other.data[1] && data[2] == other.data[2]; }
		inline vec3_t<T> operator + (vec3_t<T> const & other) const { return { data[0] + other.data[0], data[1] + other.data[1], data[2] + other.data[2] }; }
		inline vec3_t<T> operator + (T const & value) const { return { data[0] + value, data[1] + value, data[2] + value }; }
		inline vec3_t<T> operator - (vec3_t<T> const & other) const { return { data[0] - other.data[0], data[1] - other.data[1], data[2] - other.data[2] }; }
		inline vec3_t<T> operator - (T const & value) const { return { data[0] - value, data[1] - value, data[2] - value }; }
		inline vec3_t<T> operator * (T const & value) const { return { data[0] * value, data[1] * value, data[2] * value }; }
		inline vec3_t<T> operator * (mat4_t<T> const & mat) const {
			return {
				data[0] * mat[0][0] + data[1] * mat[1][0] + data[2] * mat[2][0] + mat[3][0],
				data[0] * mat[0][1] + data[1] * mat[1][1] + data[2] * mat[2][1] + mat[3][1],
				data[0] * mat[0][2] + data[1] * mat[1][2] + data[2] * mat[2][3] + mat[3][2],
			};
		}
		inline vec3_t<T> operator * (quaternion_t<T> const & quat) const {
			vec3_t<T> qw {quat.data[0], quat.data[1], quat.data[2]};
			vec3_t<T> t1, t2, t3;
			t1 = cross(*this, qw);
			t1.scale(2);
			t2 = t1.scaled(quat.data[3]);
			t3 = cross(t1, qw);
			t1 = *this;
			t1 += t2;
			t1 += t3;
			return t1;
		}
		inline vec3_t<T> operator / (T const & value) const { return { data[0] / value, data[1] / value, data[2] / value }; }
		
		inline vec3_t<T> & operator += (vec3_t<T> const & other) { data[0] += other.data[0]; data[1] += other.data[1]; data[2] += other.data[2]; return *this; }
		inline vec3_t<T> & operator += (T const & value) { data[0] += value; data[1] += value; data[2] += value; return *this; }
		inline vec3_t<T> & operator -= (vec3_t<T> const & other) { data[0] -= other.data[0]; data[1] -= other.data[1]; data[2] -= other.data[2]; return *this; }
		inline vec3_t<T> & operator -= (T const & value) { data[0] -= value; data[1] -= value; data[2] -= value; return *this; }
		inline vec3_t<T> & operator *= (T const & value) { data[0] *= value; data[1] *= value; data[2] *= value; return *this; }
		inline vec3_t<T> & operator *= (mat4_t<T> const & mat) {
			operator = ( operator * (mat) );
			return *this;
		}
		inline vec3_t<T> & operator *= (quaternion_t<T> const & quat) {
			vec3_t<T> qw {quat.data[0], quat.data[1], quat.data[2]};
			vec3_t<T> t1, t2, t3;
			t1 = cross(*this, qw);
			t1.scale(2);
			t2 = t1.scaled(quat.data[3]);
			t3 = cross(t1, qw);
			operator += (t2);
			operator += (t3);
			return *this;
		}
		inline vec3_t<T> & operator /= (T const & value) { data[0] /= value; data[1] /= value; data[2] /= value; return *this; }
		
		template <typename U> vec3_t<T> & operator = (vec3_t<U> const & other) { data[0] = other.data[0]; data[1] = other.data[1]; data[2] = other.data[2]; return *this; }
		
		inline T & operator [] (size_t i) { return data[i]; }
		inline T const & operator [] (size_t i) const { return data[i]; }
		
		inline vec3_t<T> operator - () const { return {-x(), -y(), -z()}; }
		
#if BRASSICA_PRINT_FUNCTIONS == 1
		std::string to_string() {
			return "V3[" + std::to_string(data[0]) + ", " + std::to_string(data[1]) + ", " + std::to_string(data[2]) + "]";
		}
#endif
	};
	
//================================================================================================
//------------------------------------------------------------------------------------------------
//================================================================================================
	
	template <typename T> struct vec4_t {
		
		typedef T length_t;
		T data [4] {0, 0, 0, 0};
		inline T & x() { return data[0]; }
		inline T & y() { return data[1]; }
		inline T & z() { return data[2]; }
		inline T & w() { return data[3]; }
		inline T const & x() const { return data[0]; }
		inline T const & y() const { return data[1]; }
		inline T const & z() const { return data[2]; }
		inline T const & w() const { return data[3]; }
		inline T & r() { return data[0]; }
		inline T & g() { return data[1]; }
		inline T & b() { return data[2]; }
		inline T & a() { return data[3]; }
		inline T const & r() const { return data[0]; }
		inline T const & g() const { return data[1]; }
		inline T const & b() const { return data[2]; }
		inline T const & a() const { return data[3]; }
		
		vec4_t() = default;
		vec4_t(T v) : data {v, v, v, v} {}
		vec4_t(T x, T y, T z, T w) : data {x, y, z, w} {}
		vec4_t(vec4_t const &) = default;
		vec4_t(vec4_t &&) = default;
		template <typename U> vec4_t(vec4_t<U> const & other) { data[0] = other.data[0]; data[1] = other.data[1]; data[2] = other.data[2]; data[3] = other.data[3]; }
		
		static inline T dot(vec4_t<T> const & A, vec4_t<T> const & B) {
			return A.data[0] * B.data[0] + A.data[1] * B.data[1] + A.data[2] * B.data[2] + A.data[3] * B.data[3];
		}
		
		inline T magnitude() const { return static_cast<T>(sqrt((data[0]*data[0])+(data[1]*data[1])+(data[2]*data[2])+(data[3]*data[3]))); }
		
		inline T & normalize() {
			T mag = magnitude();
			data[0] /= mag;
			data[1] /= mag;
			data[2] /= mag;
			data[3] /= mag;
			return *this;
		}
		
		inline T normalized() {
			T mag = magnitude();
			return { data[0]/mag, data[1]/mag, data[2]/mag, data[3]/mag };
		}
		
		inline vec4_t<T> & operator = (vec4_t<T> const & other) = default;
		inline vec4_t<T> & operator = (vec4_t<T> && other) = default;
		
		inline bool operator == (vec4_t<T> const & other) const { return data[0] == other.data[0] && data[1] == other.data[1] && data[2] == other.data[2] && data[3] == other.data[3]; }
		inline vec4_t<T> operator + (vec4_t<T> const & other) const { return { data[0] + other.data[0], data[1] + other.data[1], data[2] + other.data[2], data[3] + other.data[3] }; }
		inline vec4_t<T> operator + (T const & value) const { return { data[0] + value, data[1] + value, data[2] + value, data[3] + value }; }
		inline vec4_t<T> operator - (vec4_t<T> const & other) const { return { data[0] - other.data[0], data[1] - other.data[1], data[2] - other.data[2], data[3] - other.data[3] }; }
		inline vec4_t<T> operator - (T const & value) const { return { data[0] - value, data[1] - value, data[2] - value, data[3] - value }; }
		inline vec4_t<T> operator * (T const & value) const { return { data[0] * value, data[1] * value, data[2] * value, data[3] * value }; }
		inline vec4_t<T> operator * (mat4_t<T> const & mat) const {
			return {
				data[0] * mat[0][0] + data[1] * mat[1][0] + data[2] * mat[2][0] + data[3] * mat[3][0],
				data[0] * mat[0][1] + data[1] * mat[1][1] + data[2] * mat[2][1] + data[3] * mat[3][1],
				data[0] * mat[0][2] + data[1] * mat[1][2] + data[2] * mat[2][2] + data[3] * mat[3][2],
				data[0] * mat[0][3] + data[1] * mat[1][3] + data[2] * mat[2][3] + data[3] * mat[3][3],
			};
		}
		inline vec4_t<T> operator / (T const & value) const { return { data[0] / value, data[1] / value, data[2] / value, data[3] / value }; }
		
		inline vec4_t<T> & operator += (vec4_t<T> const & other) { data[0] += other.data[0]; data[1] += other.data[1]; data[2] += other.data[2]; data[3] += other.data[3]; return *this; }
		inline vec4_t<T> & operator += (T const & value) { data[0] += value; data[1] += value; data[2] += value; data[3] += value; return *this; }
		inline vec4_t<T> & operator -= (vec4_t<T> const & other) { data[0] -= other.data[0]; data[1] -= other.data[1]; data[2] -= other.data[2]; data[3] -= other.data[3]; return *this; }
		inline vec4_t<T> & operator -= (T const & value) { data[0] -= value; data[1] -= value; data[2] -= value; data[3] -= value; return *this; }
		inline vec4_t<T> & operator *= (T const & value) { data[0] *= value; data[1] *= value; data[2] *= value; data[3] *= value; return *this; }
		inline vec4_t<T> & operator *= (mat4_t<T> const & mat) {
			operator = ( operator * (mat) );
			return *this;
		}
		inline vec4_t<T> & operator /= (T const & value) { data[0] /= value; data[1] /= value; data[2] /= value; data[3] /= value; return *this; }
		
		template <typename U> vec4_t<T> & operator = (vec4_t<U> const & other) { data[0] = other.data[0]; data[1] = other.data[1]; data[2] = other.data[2]; data[3] = other.data[3]; return *this; }
		
		inline T & operator [] (size_t i) { return data[i]; }
		inline T const & operator [] (size_t i) const { return data[i]; }
		
		inline vec4_t<T> operator - () const { return {-x(), -y(), -z(), -w()}; }
		
#if BRASSICA_PRINT_FUNCTIONS == 1
		std::string to_string() {
			return "V4[" + std::to_string(data[0]) + ", " + std::to_string(data[1]) + ", " + std::to_string(data[2]) + ", " + std::to_string(data[3]) + "]";
		}
#endif
	};
	
//================================================================================================
//------------------------------------------------------------------------------------------------
//================================================================================================
	
	template <typename T> struct rect_t {
		
		typedef T length_t;
		vec2_t<T> origin {};
		vec2_t<T> extents {};
		
		inline T & x() { return origin.x(); }
		inline T & y() { return origin.y(); }
		inline T & width() { return extents.width(); }
		inline T & height() { return extents.height(); }
		inline T const & x() const { return origin.x(); }
		inline T const & y() const { return origin.y(); }
		inline T const & width() const { return extents.width(); }
		inline T const & height() const { return extents.height(); }
		
		inline rect_t() = default;
		inline rect_t(T width, T height) : extents(width, height) {}
		inline rect_t(T x, T y, T width, T height) : origin(x, y), extents(width, height) {}
		inline rect_t(vec2_t<T> const & extents) : extents(extents) {}
		inline rect_t(vec2_t<T> const & origin, vec2_t<T> const & extents) : origin(origin), extents(extents) {}
		inline rect_t(vec4_t<T> const & rect) : origin(rect[0], rect[1]), extents(rect[2], rect[3]) {}
		inline rect_t(rect_t<T> const & other) = default;
		inline rect_t(rect_t<T> && other) = default;
		
		inline rect_t<T> & operator = (rect_t<T> const & other) = default;
		inline bool operator == (rect_t<T> const & other) const { return origin == other.origin && extents = other.extents; }
		
		inline operator vec4_t<T> () const { return {origin[0], origin[1], extents[0], extents[1]}; }
		
#if BRASSICA_PRINT_FUNCTIONS == 1
		std::string to_string() {
			return "RECT[" + origin.to_string() + ", " + extents.to_string() + "]";
		}
#endif
	};
	
//================================================================================================
//------------------------------------------------------------------------------------------------
//================================================================================================
	
	template <typename T> struct quaternion_t {
		T data [4] {0, 0, 0, 1};
		inline T & x() { return data[0]; }
		inline T & y() { return data[1]; }
		inline T & z() { return data[2]; }
		inline T & w() { return data[3]; }
		
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
		quaternion_t(quaternion_t const &) = default;
		quaternion_t(quaternion_t &&) = default;
		
		inline quaternion_t<T> conjugate() const {
			return {-data[0], -data[1], -data[2], data[3]};
		}
		
		inline quaternion_t<T> inverse() const {
			quaternion_t<T> conj = conjugate();
			T m = 1 / norm();
			return {conj.data[0] * m, conj.data[1] * m, conj.data[2] * m, conj.data[3] * m};
		}
		
		inline T norm() const {
			return data[0] * data[0] + data[1] * data[1] + data[2] * data[2] + data[3] * data[3];
		}
		
		inline T mag() const {
			return sqrt(norm());
		}
		
		versor_t<T> versor() const {
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
		inline vec3_t<T> operator * (vec3_t<T> const & other) const {
			vec3_t<T> q {data[0], data[1], data[2]};
			vec3_t<T> w1 = vec3_t<T>::cross(other, q) * 2;
			return other + w1 * data[3] + vec3_t<T>::cross(w1, q);
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
		
#if BRASSICA_PRINT_FUNCTIONS == 1
		std::string to_string() {
			return "Q[" + std::to_string(data[0]) + ", " + std::to_string(data[1]) + ", " + std::to_string(data[2]) + ", " + std::to_string(data[3]) + "]";
		}
#endif
	};
	
	template <typename T> struct versor_t : public quaternion_t<T> {
		versor_t(quaternion_t<T> const & q) {
			T v = q.mag();
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
		T data [4][4] {{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}};
		
		mat4_t() = default;
		mat4_t(T v1, T v2, T v3, T v4, T v5, T v6, T v7, T v8, T v9, T v10, T v11, T v12, T v13, T v14, T v15, T v16) : data {{v1, v2, v3, v4}, {v5, v6, v7, v8}, {v9, v10, v11, v12}, {v13, v14, v15, v16}} {}
		
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
		
		static mat4_t<T> euler (vec3_t<T> const & v) {
			mat4_t<T> w {};
			T rs = sin(-v.data[0]);
			T ps = sin(v.data[1]);
			T ys = sin(-v.data[2]);
			T rc = cos(-v.data[0]);
			T pc = cos(v.data[1]);
			T yc = cos(-v.data[2]);
			w[0][0] = yc * pc;
			w[0][1] = ps * rs - pc * ys * rc;
			w[0][2] = pc * ys * rs + ps * rc;
			w[1][0] = ys;
			w[1][1] = yc * rc;
			w[1][2] = -yc * rs;
			w[2][0] = -ps * yc;
			w[2][1] = ps * ys * rc + pc * rs;
			w[2][2] = -ps * yc * rs + pc * rc;
		}
		
		static mat4_t<T> ortho(T t, T b, T l, T r, T n, T f) {
			mat4_t<T> w {};
			w[0][0] = static_cast<T>(2) / (r - l);
			w[1][1] = static_cast<T>(2) / (t - b);
			w[2][2] = static_cast<T>(-2) / (f - n);
			w[3][0] = - (r + l) / (r - l);
			w[3][1] = - (t + b) / (t - b);
			w[3][2] = - (f + n) / (f - n);
			return w;
		}
		
		static mat4_t<T> perspective(T hfov, T width, T height, T near, T far) {
			mat4_t<T> w {};
			T t1 = cos(static_cast<T>(0.5) * hfov) / sin(static_cast<T>(0.5) * hfov);
			w[0][0] = t1 * (height / width);
			w[1][1] = t1;
			w[2][2] = (far + near) / (far - near);
			w[2][3] = static_cast<T>(1);
			w[3][2] = - (static_cast<T>(2) * far * near) / (far - near);
			w[3][3] = 0;
			return w;
		}
		
		static mat4_t<T> scale(vec3_t<T> const & v) {
			mat4_t<T> w {};
			w[0][0] = v.data[0];
			w[1][1] = v.data[1];
			w[2][2] = v.data[2];
			return w;
		}
		
		static mat4_t<T> translate(vec3_t<T> const & v) {
			mat4_t<T> w {};
			w[3][0] = v.data[0];
			w[3][1] = v.data[1];
			w[3][2] = v.data[2];
			return w;
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
		
		inline T * operator [] (size_t i) { return data[i]; }
		inline T const * operator [] (size_t i) const { return data[i]; }
		
		inline operator T const * () const { return &data[0][0]; }
		
#if BRASSICA_PRINT_FUNCTIONS == 1
		std::string to_string() {
			std::string out = "M4[";
			for (int x = 0; x < 4; x++) {
				if (x > 0) out += ", ";
				out += "[";
				for (int y = 0; y < 4; y++) {
					if (y > 0) out += ", ";
					out += std::to_string(data[x][y]);
				}
				out += "]";
			}
			out += "]";
			return out;
		}
#endif
	};
	
//================================================================================================
//------------------------------------------------------------------------------------------------
//================================================================================================
	
}
