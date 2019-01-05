#include "tests.hh"

#include <random>
#include "asterales/buffer_assembly.hh"
#include "asterales/time.hh"

static std::mt19937_64 gen {std::random_device{}()};
static std::uniform_int_distribution<size_t> dist (0, 8192);
#define rng dist(gen)

#define teststr "TEST TEST TEST TEST TEST TEST"
#define teststrlen strlen(teststr)

static constexpr size_t test_count = 5000000;
#define TESTLOOP for (size_t i = 0; i < test_count; i++)

static asterales::time::keeper<asterales::time::clock_type::thread> tk;

void tests::buffer_assembly_tests() {
	tlog << "STARTING BUFFER ASSEMBLY TESTS\n";
	asterales::buffer_assembly buf, buf2;
	TEST(buf.size() == 0);
	buf.resize(0);
	TEST(buf.capacity() != 0);
	tlogi << "RESIZE: ";
	{
		tk.mark();
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
		auto tm = tk.mark();
		tlog << tm.sec() << " sec";
	}
	tlogi << "RESERVE: ";
	{
		tk.mark();
		TESTLOOP {
			size_t size = rng;
			buf.reserve(size);
			TEST(buf.size() == 0);
			TEST(buf.capacity() >= size);
		}
		auto tm = tk.mark();
		tlog << tm.sec() << " sec";
	}
	tlogi << "RESERVE + SHRINK: ";
	{
		tk.mark();
		TESTLOOP {
			buf.resize(1);
			buf.shrink();
			size_t size = rng;
			buf.reserve(size);
			TEST(buf.size() == 1);
			TEST(buf.capacity() >= size);
		}
		auto tm = tk.mark();
		tlog << tm.sec() << " sec";
	}
	tlogi << "DATA + SHRINK + RESIZE: ";
	{
		tk.mark();
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
		auto tm = tk.mark();
		tlog << tm.sec() << " sec";
	}
	tlogi << "TRANSFER_TO: ";
	{
		tk.mark();
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
		auto tm = tk.mark();
		tlog << tm.sec() << " sec";
	}
	tlogi << "WRITE + READ: ";
	{
		tk.mark();
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
		auto tm = tk.mark();
		tlog << tm.sec() << " sec";
	}
	tlog << "\nBUFFER ASSEMBLY TESTS DONE";
}
