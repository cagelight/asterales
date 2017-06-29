#include "asterid/strops.hh"

#include <cstdarg>

static constexpr size_t strf_startlen = 256;
std::string asterid::strf(char const * fmt, ...) noexcept {
	
	va_list va;
	va_start(va, fmt);
	char * tmp_buf = reinterpret_cast<char *>(malloc(strf_startlen));
	tmp_buf[strf_startlen - 1] = 0;
	size_t req_size = vsnprintf(tmp_buf, strf_startlen, fmt, va);
	va_end(va);
	if (req_size >= strf_startlen) {
		tmp_buf = reinterpret_cast<char *>(realloc(tmp_buf, req_size+1));
		va_start(va, fmt);
		vsprintf(tmp_buf, fmt, va);
		va_end(va);
		tmp_buf[req_size] = 0;
	}
	std::string r = {tmp_buf};
	free(tmp_buf);
	
	return {r};
}

static thread_local char * vas_buffers [VAS_HISTORY_SIZE] {0};
static thread_local int vas_cur = 0;
char const * asterid::vas(char const * fmt, ...) noexcept {

	va_list va;
	va_start(va, fmt);
	size_t size = vsnprintf(nullptr, 0, fmt, va) + 1;
	va_end(va);
	
	if (++vas_cur >= VAS_HISTORY_SIZE) vas_cur = 0;
	char * & vas_buf = vas_buffers[vas_cur];
	vas_buf = reinterpret_cast<char *>(realloc(vas_buf, size));
	
	va_start(va, fmt);
	vsnprintf(vas_buf, size, fmt, va);
	va_end(va);
	
	return vas_buf;
}
