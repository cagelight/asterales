#include "tests.hh"

#include <mutex>
#include <iostream>

static std::mutex log_mut {};
void util::log(log_level, std::string const & str) {
	log_mut.lock();
	std::cout << str << std::endl;
	log_mut.unlock();
}


int main(int argc, char * * argv) {
	if (argc != 2) {
		tlog << "1 argument is required";
		return 1;
	}
	std::string arg = argv[1];
	if (arg == "buffer_assembly") {
		tests::buffer_assembly_tests();
	} else if (arg == "threadpool") {
		tests::threadpool_tests();
	} else if (arg == "codon") {
		tests::codon_tests();
	} else if (arg == "brassica") {
		tests::brassica_tests();
	} else if (arg == "strop") {
		tests::strop_tests();
	} else if (arg == "signal") {
		tests::signal_tests();
	} else {
		tlog << "unknown argument: \"" << arg << "\"";
		tlog << "must be one of:\n> brassica\n> buffer_assembly\n> codon\n> strop\n> threadpool";
		return 1;
	}
	return 0;
}
