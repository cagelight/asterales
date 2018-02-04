#include "tests.hh"

#include <random>
#include "asterid/buffer_assembly.hh"

static std::mt19937_64 gen {std::random_device{}()};
static std::uniform_int_distribution<size_t> dist (0, 4096);
#define rng dist(gen)

#define teststr "TEST TEST TEST TEST TEST TEST"
#define teststrlen strlen(teststr)

static constexpr size_t test_count = 1<<22;
#define TESTLOOP for (size_t i = 0; i < test_count; i++)

void tests::buffer_assembly_tests() {
	scilogi << "STARTING BUFFER ASSEMBLY TESTS";
	asterid::buffer_assembly buf, buf2;
	TEST(buf.size() == 0);
	buf.resize(0);
	TEST(buf.capacity() != 0);
	scilogi << "RESIZE";
	TESTLOOP {
		size_t size = rng;
		buf.resize(size);
		TEST(buf.size() == size);
		TEST(buf.capacity() >= size);
	}
	buf.clear();
	TEST(buf.size() == 0);
	TEST(buf.capacity() != 0);
	buf.shrink();
	TEST(buf.size() == 0);
	TEST(buf.capacity() != 0);
	scilogi << "RESERVE";
	TESTLOOP {
		size_t size = rng;
		buf.reserve(size);
		TEST(buf.size() == 0);
		TEST(buf.capacity() >= size);
	}
	scilogi << "RESERVE + SHRINK";
	TESTLOOP {
		buf.resize(1);
		buf.shrink();
		size_t size = rng;
		buf.reserve(size);
		TEST(buf.size() == 1);
		TEST(buf.capacity() >= size);
	}
	scilogi << "DATA + SHRINK + RESIZE";
	TESTLOOP {
		buf.clear();
		buf.write(teststr);
		buf.shrink();
		size_t size = rng;
		buf.resize(size);
		TEST(buf.size() == size);
		TEST(buf.capacity() >= size);
		TEST(!strncmp(teststr, (char const *)buf.data(), size));
	}
	scilogi << "TRANSFER_TO";
	TESTLOOP {
		std::uniform_int_distribution<size_t> dist2 (0, teststrlen + 1);
		buf.clear();
		buf.write(teststr);
		buf.shrink();
		buf2.clear();
		buf2.write(teststr);
		buf2.shrink();
		buf2.transfer_to(buf, dist2(gen));
		TEST(buf.size() + buf2.size() == 2 * teststrlen);
	}
	scilogi << "WRITE + READ";
	TESTLOOP {
		buf.clear();
		size_t rng1 = rng;
		size_t rng2 = rng;
		size_t rng3 = rng;
		size_t rng4 = rng;
		buf.write(rng1);
		buf.write(rng2);
		buf.write(rng3);
		buf.write(rng1);
		buf.write(rng4);
		TEST(buf.read<size_t>() == rng1);
		TEST(buf.precheck<size_t>());
		TEST(buf.read<size_t>() == rng2);
		TEST(buf.read<size_t>() == rng3);
		buf.discard(sizeof(size_t));
		TEST(buf.read<size_t>() == rng4);
		TEST(buf.size() == 0);
	}
	scilogi << "BUFFER ASSEMBLY TESTS DONE";
}
