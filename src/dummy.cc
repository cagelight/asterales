#include "asterid/brassica.hh"
#include "asterid/istring.hh"
#include "asterid/synchro.hh"
#include "asterid/time.hh"
#include "asterid/bspacker.hh"
#include "asterid/linq.hh"
#include "asterid/hyena.hh"

#include <vector>

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
}

void istring_test () {
	istring str {"TEST"};
	std::string str2 {"TEST"};
}

void linq_test () {
	std::vector<int> test1 {1, 2, 3, 4, 5, 6, 7, 8, 9};
	//asterid::linq<std::vector<int>> linq1 {test1};
}

struct test_panel : public hyena::panel<int> {
protected:
	virtual void layout_eval() override {}
};

void hyena_test() {
	hyena::hbox<test_panel> test_layout;
}
