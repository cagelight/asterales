#include "asterid/brassica.hh"

using namespace asterid::brassica;

// dummy file -- used for validation

struct brassica_test_s {
	vec2_t<double> testv2;
	vec3_t<double> testv3;
	mat4_t<double> testm4;
};

void brassica_test () {
	brassica_test_s test;
	test.testm4.scale(5);
};
