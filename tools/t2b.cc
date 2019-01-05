#include "asterales/aeon.hh"

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
	char * buf_in = new char [aeon_stat.st_size];
	size_t r = fread(buf_in, aeon_stat.st_size, 1, aeon_in);
	assert(r == 1);
	fclose(aeon_in);
	
	asterales::aeon::object aeon_dat = asterales::aeon::parse_text(buf_in);
	delete [] buf_in;
	
	asterales::buffer_assembly buf_out = aeon_dat.serialize_binary();
	FILE * aeon_out = fopen(argv[2], "wb");
	size_t w = fwrite(buf_out.data(), buf_out.size(), 1, aeon_out);
	assert(w == 1);
	fclose(aeon_out);
	
	return 0;
}
