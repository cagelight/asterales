#include "asterid/noise.hh"

#include <random>

typedef asterid::brassica::vec3_t<double> grad_t;

static grad_t constexpr grad3[] = {
	{1,1,0}, {-1,1,0}, {1,-1,0}, {-1,-1,0},
	{1,0,1}, {-1,0,1}, {1,0,-1}, {-1,0,-1},
	{0,1,1}, {0,-1,1}, {0,1,-1}, {0,-1,-1}
};

static double const F2 = 0.5 * (sqrt(3.0) - 1.0);
static double const G2 = (3.0 - sqrt(3.0)) / 6.0;
static double constexpr F3 = 1.0 / 3.0;
static double constexpr G3 = 1.0 / 6.0;
static double const F4 = (sqrt(5.0) - 1.0) / 4.0;
static double const G4 = (5.0 - sqrt(5.0)) / 20.0;

double asterid::simplex::generate(double x, double y) const {

	double n0 = 0.0, n1 = 0.0, n2 = 0.0;
	double s = (x + y) * F2;
	int_fast64_t i = x + s;
	int_fast64_t j = y + s;
	double t = (i + j) * G2;
	double X0 = i - t;
	double Y0 = j - t;
	double x0 = x - X0;
	double y0 = y - Y0;
	
	uint_fast8_t i1 = 0, j1 = 0;
	if (x0 > y0) {
		i1 = 1;
		j1 = 0;
	}
	else {
		i1 = 0;
		j1 = 1;
	}
	
	double x1 = x0 - i1 + G2;
	double y1 = y0 - j1 + G2;
	double x2 = x0 - 1.0 + 2.0 * G2;
	double y2 = y0 - 1.0 + 2.0 * G2;
	
	uint_fast8_t ii = i & 0xFF;
	uint_fast8_t jj = j & 0xFF;
	uint_fast8_t gi0 = perm12[ii + perm[jj]];
	uint_fast8_t gi1 = perm12[ii + i1 + perm[jj + j1]];
	uint_fast8_t gi2 = perm12[ii + 1 + perm[jj + 1]];
	
	double t0 = 0.5 - x0 * x0 - y0 * y0;
	if (t0 < 0) n0 = 0.0;
	else {
		t0 *= t0;
		n0 = t0 * t0 * grad_t::dot(grad3[gi0], {x0, y0, 0});
	}
	double t1 = 0.5 - x1 * x1 - y1 * y1;
	if (t1 < 0) n1 = 0.0;
	else {
		t1 *= t1;
		n1 = t1 * t1 * grad_t::dot(grad3[gi1], {x1, y1, 0}); 
	}
	double t2 = 0.5 - x2 * x2 - y2 * y2;
	if (t2 < 0) n2 = 0.0;
	else {
		t2 *= t2;
		n2 = t2 * t2 * grad_t::dot(grad3[gi2], {x2, y2, 0}); 
	}
	
	return 70.0 * (n0 + n1 + n2);
}

double asterid::simplex::generate(double x, double y, double z) const {
	
	double n0, n1, n2, n3;
	double s = (x + y + z) * F3;
	int_fast64_t i = x + s;
	int_fast64_t j = y + s;
	int_fast64_t k = z + s;
	double t = (i + j + k) * G3;
	double X0 = i - t;
	double Y0 = j - t;
	double Z0 = k - t;
	double x0 = x - X0;
	double y0 = y - Y0;
	double z0 = z - Z0;
	
	uint_fast8_t i1, j1, k1;
	uint_fast8_t i2, j2, k2;
	if (x0 >= y0) {
		if (y0 >= z0) {
			i1 = 1;
			j1 = 0;
			k1 = 0;
			i2 = 1;
			j2 = 1;
			k2 = 0;
		} else if (x0 >= z0) {
			i1 = 1;
			j1 = 0;
			k1 = 0;
			i2 = 1;
			j2 = 0;
			k2 = 1;
		} else {
			i1 = 0;
			j1 = 0;
			k1 = 1;
			i2 = 1;
			j2 = 0;
			k2 = 1;
		}
	} else {	
		if (y0 < z0) {
			i1 = 0;
			j1 = 0;
			k1 = 1;
			i2 = 0;
			j2 = 1;
			k2 = 1;
		} else if (x0 < z0) {
			i1 = 0;
			j1 = 1;
			k1 = 0;
			i2 = 0;
			j2 = 1;
			k2 = 1;
		} else {
			i1 = 0;
			j1 = 1;
			k1 = 0;
			i2 = 1;
			j2 = 1;
			k2 = 0;
		}
	}
	
	double x1 = x0 - i1 + G3;
	double y1 = y0 - j1 + G3;
	double z1 = z0 - k1 + G3;
	double x2 = x0 - i2 + 2.0 * G3;
	double y2 = y0 - j2 + 2.0 * G3;
	double z2 = z0 - k2 + 2.0 * G3;
	double x3 = x0 - 1.0 + 3.0 * G3;
	double y3 = y0 - 1.0 + 3.0 * G3;
	double z3 = z0 - 1.0 + 3.0 * G3;
	
	uint_fast8_t ii = i & 255;
	uint_fast8_t jj = j & 255;
	uint_fast8_t kk = k & 255;
	uint_fast8_t gi0 = perm12[ii + perm[jj + perm[kk]]];
	uint_fast8_t gi1 = perm12[ii + i1 + perm[jj + j1 + perm[kk + k1]]];
	uint_fast8_t gi2 = perm12[ii + i2 + perm[jj + j2 + perm[kk + k2]]];
	uint_fast8_t gi3 = perm12[ii + 1 + perm[jj + 1 + perm[kk + 1]]];
	
	double t0 = 0.6 - x0 * x0 - y0 * y0 - z0 * z0;
	if (t0 < 0) n0 = 0.0;
	else {
		t0 *= t0;
		n0 = t0 * t0 * vec3_t::dot(grad3[gi0],{x0, y0, z0});
	}
	double t1 = 0.6 - x1 * x1 - y1 * y1 - z1 * z1;
	if (t1 < 0) n1 = 0.0;
	else {
		t1 *= t1;
		n1 = t1 * t1 * vec3_t::dot(grad3[gi1],{x1, y1, z1});
	}
	double t2 = 0.6 - x2 * x2 - y2 * y2 - z2 * z2;
	if (t2 < 0) n2 = 0.0;
	else {
		t2 *= t2;
		n2 = t2 * t2 * vec3_t::dot(grad3[gi2],{x2, y2, z2});
	}
	double t3 = 0.6 - x3 * x3 - y3 * y3 - z3 * z3;
	if (t3 < 0) n3 = 0.0;
	else {
		t3 *= t3;
		n3 = t3 * t3 * vec3_t::dot(grad3[gi3],{x3, y3, z3});
	}
	
	return 32.0 * (n0 + n1 + n2 + n3);
	
}
