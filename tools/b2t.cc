#include "asterid/aeon.hh"

#include <cassert>
#include <sys/stat.h>
#include <cerrno>
#include <cstdio>

int main(int argc, char * * argv) {
	if (argc != 3) {
		printf("two arguments required\n");
		return 1;
	}
	
	struct stat aeon_stat;
	if (stat(argv[1], &aeon_stat) != 0) {
		printf("file \"%s\" is not valid (%i)\n", argv[1], errno);
		return 1;
	}
	FILE * aeon_in = fopen(argv[1], "rb");
	asterid::buffer_assembly buf_in;
	buf_in.resize(aeon_stat.st_size);
	size_t r = fread(buf_in.data(), aeon_stat.st_size, 1, aeon_in);
	assert(r == 1);
	fclose(aeon_in);
	
	asterid::aeon::object aeon_dat = asterid::aeon::parse_binary(buf_in);
	
	std::string buf_out = aeon_dat.serialize_text();
	FILE * aeon_out = fopen(argv[2], "wb");
	size_t w = fwrite(buf_out.data(), buf_out.size(), 1, aeon_out);
	assert(w == 1);
	fclose(aeon_out);
	
	return 0;
}
