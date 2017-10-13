#include "asterid/buffer_assembly.hh"

#include <cstdlib>

using namespace asterid;

static constexpr size_t default_size = 256;
#define datav reinterpret_cast<void *>(data_)
#define offsv reinterpret_cast<void *>(offset_)
#define voidp(ptr) reinterpret_cast<void *>(ptr)
#define datap(ptr) reinterpret_cast<byte_t *>(ptr)
#define odiff (offset_ - data_)

buffer_assembly::buffer_assembly() : data_(datap(malloc(default_size))), offset_(data_), size_(0), reserve_size_(default_size) {}
buffer_assembly::buffer_assembly(const_iterator begin, const_iterator end) : data_(datap(malloc(end - begin))), offset_(data_), size_(end - begin), reserve_size_(size_) {
	memcpy(data_, begin, size_);
}
buffer_assembly::buffer_assembly(buffer_assembly const & other) : data_(datap(malloc(other.size_))), offset_(data_), size_(other.size_), reserve_size_(size_) {
	memcpy(data_, other.offset_, size_);
}
buffer_assembly::buffer_assembly(buffer_assembly && other) : data_(other.data_), offset_(other.offset_), size_(other.size_), reserve_size_(other.reserve_size_) {
	other.offset_ = other.data_ = nullptr;
	other.reserve_size_ = other.size_ = 0;
}
buffer_assembly::~buffer_assembly() { if (data_) free(data_); }

buffer_assembly & buffer_assembly::operator = (buffer_assembly const & other) {
	offset_ = data_ = datap(realloc(data_, other.size_));
	reserve_size_ = size_  = other.size_;
	memcpy(data_, other.offset_, size_);
	return *this;
}

buffer_assembly & buffer_assembly::operator = (buffer_assembly && other) {
	if (data_) free(data_); 
	data_ = other.data_;
	offset_ = other.offset_;
	size_ = other.size_;
	reserve_size_ = other.reserve_size_;
	other.offset_ = other.data_ = nullptr;
	other.reserve_size_ = other.size_ = 0;
	return *this;
}

bool buffer_assembly::operator == (buffer_assembly const & other) {
	if (size_ != other.size_) return false;
	return memcmp(offset_, other.offset_, size_) == 0;
}

void buffer_assembly::realign() {
	if (data_ == offset_) return;
	memmove(datav, offsv, size_);
}

void buffer_assembly::reserve(size_t new_size) {
	realign();
	if (new_size <= reserve_size_) return;
	reserve_size_ = new_size;
	offset_ = data_ = datap(realloc(data_, reserve_size_));
}

void buffer_assembly::resize(size_t new_size) {
	realign();
	if (new_size == reserve_size_) return;
	reserve_size_ = new_size;
	offset_ = data_ = datap(realloc(data_, reserve_size_));
	if (size_ < reserve_size_)
		memset(data_ + size_, 0, reserve_size_ - size_);
	size_ = reserve_size_;
}

void buffer_assembly::shrink() {
	realign();
	if (reserve_size_ == size_) return;
	reserve_size_ = size_;
	offset_ = data_ = datap(realloc(data_, reserve_size_));
}

void buffer_assembly::clear() {
	offset_ = data_;
	size_ = 0;
}

static constexpr uint8_t hmask = 0b11110000;
static constexpr uint8_t lmask = 0b00001111;

std::string buffer_assembly::hex() const {
	std::string ret {};
	ret.reserve(size_ * 2);
	for (byte_t ch : *this) {
		char vh = (ch & hmask) >> 4;
		char vl = ch & lmask;
		ret += vh > 9 ? 55 + vh : 48 + vh;
		ret += vl > 9 ? 55 + vl : 48 + vl;
	}
	return ret;
}

std::string buffer_assembly::hexlow() const {
	std::string ret {};
	ret.reserve(size_ * 2);
	for (byte_t ch : *this) {
		char vh = (ch & hmask) >> 4;
		char vl = ch & lmask;
		ret += vh > 9 ? 87 + vh : 48 + vh;
		ret += vl > 9 ? 87 + vl : 48 + vl;
	}
	return ret;
}

void buffer_assembly::write(byte_t const * src, size_t size) {
	reserve(size_ + size);
	memcpy(offset_ + size_, src, size);
	size_ += size;
}

void buffer_assembly::read(byte_t * dest, size_t size) {
	memcpy(dest, offset_, size);
	offset_ += size;
	size_ -= size;
}

void buffer_assembly::discard(size_t size) {
	offset_ += size;
}

std::string buffer_assembly::to_string() const {
	return {reinterpret_cast<char const *>(offset_), size_};
}
