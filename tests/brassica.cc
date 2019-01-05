#include "asterales/brassica.hh"
#include "tests.hh"

#include <random>

typedef double test_t;

namespace b = asterales::brassica;
typedef b::vec3_t<test_t> vec3_t;
typedef b::vec4_t<test_t> vec4_t;
typedef b::rect_t<test_t> rect_t;
typedef b::la::ray_t<test_t> ray_t;
typedef b::la::plane_t<test_t> plane_t;

static std::mt19937_64 gen {std::random_device{}()};
static std::uniform_real_distribution<test_t> dist (0, 8192);
#define rng dist(gen)

static constexpr size_t test_count = 1<<22;
#define TESTLOOP for (size_t i = 0; i < test_count; i++)

bool tolerant_equal(test_t A, test_t B, test_t ERR = 0.00000001) {
	if (A == B) return true;
	test_t diff = A - B;
	if (diff < 0) diff = -diff;
	if (diff < ERR) return true;
	return false;
}

void tests::brassica_tests() {
	tlog << "STARTING BRASSICA TESTS";
	TESTLOOP {
		rect_t a {rng, rng, rng, rng};
		rect_t b {rng, rng, rng, rng};
		rect_t c = a;
		a.center_in(b);
		TEST(tolerant_equal((a.origin[0] - b.origin[0]) * 2 + a.extents[0], b.extents[0]));
		TEST(tolerant_equal((a.origin[1] - b.origin[1]) * 2 + a.extents[1], b.extents[1]));
		a = c;
		a.fit_in(b);
		TEST(tolerant_equal(a.ratio(), c.ratio()));
		TEST(tolerant_equal(b.extents[0], a.extents[0]) || tolerant_equal(a.extents[1], b.extents[1]));
	}
}
