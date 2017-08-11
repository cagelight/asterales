#include "asterid/brassica.hh"
#include "asterid/istring.hh"
#include "asterid/synchro.hh"
#include "asterid/time.hh"
#include "asterid/bspacker.hh"

using namespace asterid;
using namespace asterid::brassica;

// dummy file -- used for validation

struct brassica_test_s {
	vec2_t<double> testv2;
	vec3_t<double> testv3;
	mat4_t<double> testm4;
};

void brassica_test () {
	brassica_test_s test;
	//test.testm4.scale(5);
	test.testm4 = {};
};

void istring_test () {
	istring str {"TEST"};
	std::string str2 {"TEST"};
	
	istring_convertible conv {str};
	istring_convertible conv2 {str2};
}
