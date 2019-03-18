#include "tests.hh"

#include <random>
#include "asterales/buffer_assembly.hh"
#include "asterales/time.hh"

static std::mt19937_64 gen {std::random_device{}()};
static std::uniform_int_distribution<size_t> dist (0, 8192);
#define rng dist(gen)

#define teststr "TEST TEST TEST TEST TEST TEST"
#define teststrlen strlen(teststr)

static constexpr size_t test_count = 50000;
#define TESTLOOP for (size_t i = 0; i < test_count; i++)

static asterales::time::keeper<asterales::time::clock_type::thread> tk;

struct some_other_object : public asterales::serializable {
	uint16_t num1;
	uint32_t num2;
	
	some_other_object() = default;
	some_other_object(uint16_t n1, uint32_t n2) : num1(n1), num2(n2) {}
	
	virtual void serialize(asterales::serializer & buf) const override {
		buf << num1 << num2;
	}
	
	virtual void deserialize(asterales::serializer & buf) override {
		buf >> num1 >> num2;
	}
};

struct some_object : public asterales::serializable {
	uint8_t num;
	std::string str;
	std::vector<some_other_object> vec;
	
	virtual void serialize(asterales::serializer & buf) const override {
		buf << num << str;
		buf.write_container<uint8_t>(vec);
	}
	
	virtual void deserialize(asterales::serializer & buf) override {
		buf >> num >> str;
		buf.read_container<uint8_t>(vec);
	}
	
	void print() {
		tlog << "  " << num;
		tlog << "  " << str;
		tlogi << "  ";
		for (some_other_object const & soo : vec) tlogi << "(" << soo.num1 << ", " << soo.num2 << ") ";
		tlog << "\n";
	}
};

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
			buf.write(teststr, teststrlen);
			buf.shrink();
			buf2.clear();
			buf2.write(teststr, teststrlen);
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
	tlog << "SERIALIZATION: ";
	{
		std::uniform_int_distribution<uint8_t> dist8 (0, std::numeric_limits<uint8_t>::max());
		std::uniform_int_distribution<uint16_t> dist16 (0, std::numeric_limits<uint16_t>::max());
		std::uniform_int_distribution<uint32_t> dist32 (0, std::numeric_limits<uint32_t>::max());
		
		some_object so1, so2;
		
		tlog << "SERIALIZE IN:";
		so1.num = dist8(gen);
		so1.str = teststr;
		for (size_t i = 0; i < 8; i++) so1.vec.emplace_back(dist16(gen), dist32(gen));
		so1.print();
		
		asterales::serializer sobuf;
		sobuf.write(so1);
		tlog << "SERIALIZED HEX: " << sobuf.buffer.hex();
		
		tlog << "SERIALIZE OUT:";
		so2.deserialize(sobuf);
		so2.print();
	}
	
	tlog << "\nBUFFER ASSEMBLY TESTS DONE";
}
