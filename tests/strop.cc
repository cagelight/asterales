#include "asterales/strop.hh"
#include "tests.hh"

#include <random>

static std::mt19937_64 gen {std::random_device{}()};
#define rng dist(gen)

static constexpr size_t test_count = 1<<22;
#define TESTLOOP for (size_t i = 0; i < test_count; i++)

void tests::strop_tests() {
	
}
