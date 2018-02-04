#include "tests.hh"

#include "asterid/codon.hh"

#include <random>

enum struct c1te {
	inc,
	dec
};

struct c1tr {
	c1te operator () (c1te) const {
		return c1te::dec;
	}
};

void tests::codon_tests() {
	/*
	asterid::codon::chain_1bit<c1te> c1t {20};
	
	TEST(c1t.size() == 20);
	TEST(c1t.data().size() == 3);
	
	c1t.mutate_all(c1tr {});
	c1t.iterate([](c1te v){
		TEST(v == c1te::dec);
	});
	*/
}
