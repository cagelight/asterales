#include "asterales/aeon.hh"

#include <iomanip>
#include <sstream>
#include <limits>

using namespace asterales;

static aeon::object null_ {};
aeon::object const & aeon::null {null_};

#define pthrow throw aeon::exception::parse {}
#define pcheck(type) if (!buf.precheck<type>()) pthrow
#define ncheck(size) if (!buf.precheck(size)) pthrow

// ================================================================================================
// ------------------------------------------------------------------------------------------------
// ================================================================================================
// CONSTRUCTORS

aeon::object::object(type t) : t_(t) {
	switch(t_) {
		case type::none: break;
		case type::boolean: data.boolean = false; break;
		case type::integer: data.num_int = 0; break;
		case type::real: data.num_real = 0; break;
		case type::string: data.str = new str_t {}; break;
		case type::array: data.ary = new ary_t {}; break;
		case type::map: data.map = new map_t {}; break;
		case type::binary: data.bin = new bin_t {}; break;
	}
}

aeon::object::~object() {
	switch(t_) {
		case type::none:
		case type::boolean:
		case type::integer:
		case type::real: break;
		case type::string: delete data.str; break;
		case type::array: delete data.ary; break;
		case type::map: delete data.map; break;
		case type::binary: delete data.bin; break;
	}
}

aeon::object::object(bool v) : t_(type::boolean) {
	data.boolean = v;
}

aeon::object::object(int_t v) : t_(type::integer) {
	data.num_int = v;
}

aeon::object::object(real_t v) : t_(type::real) {
	data.num_real = v;
}

aeon::object::object(str_t const & v) : t_(type::string) {
	data.str = new str_t(v);
}

aeon::object::object(str_t && v) : t_(type::string) {
	data.str = new str_t(std::forward<str_t &&>(v));
}

aeon::object::object(ary_t const & v) : t_(type::array) {
	data.ary = new ary_t(v);
}

aeon::object::object(ary_t && v) : t_(type::array) {
	data.ary = new ary_t(std::forward<ary_t &&>(v));
}

aeon::object::object(map_t const & v) : t_(type::map) {
	data.map = new map_t(v);
}

aeon::object::object(map_t && v) : t_(type::map) {
	data.map = new map_t(std::forward<map_t &&>(v));
}

aeon::object::object(bin_t const & v) : t_(type::binary) {
	data.bin = new bin_t(v);
}

aeon::object::object(bin_t && v) : t_(type::binary) {
	data.bin = new bin_t(std::forward<bin_t &&>(v));
}

aeon::object::object(object const & other) : t_(other.t_) {
	switch(t_) {
		case type::none: break;
		case type::boolean: data.boolean = other.data.boolean; break;
		case type::integer: data.num_int = other.data.num_int; break;
		case type::real: data.num_real = other.data.num_real; break;
		case type::string: data.str = new str_t (*other.data.str); break;
		case type::array: data.ary = new ary_t (*other.data.ary); break;
		case type::map: data.map = new map_t (*other.data.map); break;
		case type::binary: data.bin = new bin_t (*other.data.bin); break;
	}
}

aeon::object::object(object && other) : t_(other.t_) {
	data = other.data;
	other.t_ = type::none;
}

// ================================================================================================
// ------------------------------------------------------------------------------------------------
// ================================================================================================
// ASSIGNMENT

aeon::object & aeon::object::operator = (object const & other) {
	t_ = other.t_;
	switch(t_) {
		case type::none: break;
		case type::boolean: data.boolean = other.data.boolean; break;
		case type::integer: data.num_int = other.data.num_int; break;
		case type::real: data.num_real = other.data.num_real; break;
		case type::string: data.str = new str_t (*other.data.str); break;
		case type::array: data.ary = new ary_t (*other.data.ary); break;
		case type::map: data.map = new map_t (*other.data.map); break;
		case type::binary: data.bin = new bin_t (*other.data.bin); break;
	}
	return *this;
}

aeon::object & aeon::object::operator = (object && other) {
	t_ = other.t_;
	data = other.data;
	other.t_ = type::none;
	return *this;
}

// ================================================================================================
// ------------------------------------------------------------------------------------------------
// ================================================================================================
// VALUE EXTRACTION

bool & aeon::object::boolean() {
	if (t_ != type::boolean) *this = object {type::boolean};
	return data.boolean;
}

static bool constexpr null_bool = false;
bool const & aeon::object::boolean() const {
	if (t_ != type::boolean) return null_bool;
	return data.boolean;
}

aeon::int_t & aeon::object::integer() {
	if (t_ != type::integer) *this = object {type::integer};
	return data.num_int;
}

static aeon::int_t constexpr null_int = 0;
aeon::int_t const & aeon::object::integer() const {
	if (t_ != type::integer) return null_int;
	return data.num_int;
}

aeon::real_t & aeon::object::real() {
	if (t_ != type::real) *this = object {type::real};
	return data.num_real;
}

static aeon::real_t constexpr null_real = 0;
aeon::real_t const & aeon::object::real() const {
	if (t_ != type::real) return null_real;
	return data.num_real;
}

aeon::str_t & aeon::object::string() {
	if (t_ != type::string) *this = object {type::string};
	return *data.str;
}

static aeon::str_t const null_string {};
aeon::str_t const & aeon::object::string() const {
	if (t_ != type::string) return null_string;
	return *data.str;
}

aeon::ary_t & aeon::object::array() {
	if (t_ != type::array) *this = object {type::array};
	return *data.ary;
}

static aeon::ary_t const null_array {};
aeon::ary_t const & aeon::object::array() const {
	if (t_ != type::array) return null_array;
	return *data.ary;
}

aeon::map_t & aeon::object::map() {
	if (t_ != type::map) *this = object {type::map};
	return *data.map;
}

static aeon::map_t const null_map {};
aeon::map_t const & aeon::object::map() const {
	if (t_ != type::map) return null_map;
	return *data.map;
}

aeon::bin_t & aeon::object::binary() {
	if (t_ != type::binary) *this = object {type::binary};
	return *data.bin;
}

static aeon::bin_t const null_bin {};
aeon::bin_t const & aeon::object::binary() const {
	if (t_ != type::binary) return null_bin;
	return *data.bin;
}

bool aeon::object::as_boolean() const {
	switch(t_) {
		default: return false;
		case type::boolean: return data.boolean;
		case type::integer: return data.num_int != 0;
		case type::real: return data.num_real != 0;
		case type::string: return !data.str->empty();
	}
}

aeon::int_t aeon::object::as_integer() const {
	switch(t_) {
		default: return 0;
		case type::boolean: return data.boolean ? 1 : 0;
		case type::integer: return data.num_int;
		case type::real: return data.num_real;
		case type::string: return strtoll(data.str->data(), nullptr, 10);
	}
}

aeon::real_t aeon::object::as_real() const {
	switch(t_) {
		default: return 0;
		case type::boolean: return data.boolean ? 1 : 0;
		case type::integer: return data.num_int;
		case type::real: return data.num_real;
		case type::string: return strtod(data.str->data(), nullptr);
	}
}

aeon::str_t aeon::object::as_string() const {
	switch(t_) {
		default: return "";
		case type::boolean: return std::to_string(data.boolean);
		case type::integer: return std::to_string(data.num_int);
		case type::real: return std::to_string(data.num_real);
		case type::string: return *data.str;
	}
}


// ================================================================================================
// ------------------------------------------------------------------------------------------------
// ================================================================================================
// INDEX OPERATORS

aeon::object & aeon::object::operator [] (size_t i) {
	if (t_ != type::array) *this = object {type::array};
	if (data.ary->size() <= i) data.ary->resize(i + 1);
	return data.ary->at(i);
}

aeon::object const & aeon::object::operator [] (size_t i) const {
	if (t_ != type::array) return null;
	if (data.ary->size() <= i) return null;
	return data.ary->at(i);
}

aeon::object & aeon::object::operator [] (aeon::str_t const & key) {
	if (t_ != type::map) *this = object {type::map};
	return data.map->operator[](key);
}

aeon::object const & aeon::object::operator [] (aeon::str_t const & key) const {
	if (t_ != type::map) return null;
	auto const & i = data.map->find(key);
	if (i == data.map->end()) return null;
	return i->second;
}

// ================================================================================================
// ------------------------------------------------------------------------------------------------
// ================================================================================================
// SERIALIZE TEXT

static std::string serialize_aeon_text_real(aeon::real_t v) {
	std::ostringstream r {};
	//r.precision(std::numeric_limits<aeon::real_t>::max_digits10);
	r << v;
	return r.str();
}

static std::string serialize_aeon_text_string(aeon::str_t const & v) {
	std::ostringstream r {};
	r << "\"";
	for (uint8_t c : v) switch (c) {
		case '"': r << "\\\""; break;
		case '\\': r << "\\\\"; break;
		case '\b': r << "\\b"; break;
		case '\f': r << "\\f"; break;
		case '\n': r << "\\n"; break;
		case '\r': r << "\\r"; break;
		case '\t': r << "\\t"; break;
		default:
			if (c < 32)
				r << "\\u" << std::hex << std::setw(4) << std::setfill('0') << static_cast<int>(c);
			else r << c;
		break;
	}
	r << "\"";
	return r.str();
}

static std::string serialize_aeon_text_array(aeon::ary_t const & v) {
	std::ostringstream r {};
	r << '[';
	bool first = true;
	for (auto const & i : v) {
		if (!first) r << ","; else first = false;
		r << i;
	}
	r << ']';
	return r.str();
}

static std::string serialize_aeon_text_map(aeon::map_t const & v) {
	std::ostringstream r {};
	r << '{';
	bool first = true;
	for (auto const & i : v) {
		if (!first) r << ","; else first = false;
		r << serialize_aeon_text_string(i.first);
		r << ":";
		r << i.second;
	}
	r << '}';
	return r.str();
}

std::string aeon::object::serialize_text() const {
	switch(t_) {
		default:
		case type::none:
			return "null";
		case type::boolean:
			return data.boolean ? "true" : "false";
		case type::integer:
			return std::to_string(data.num_int);
		case type::real:
			return serialize_aeon_text_real(data.num_real);
		case type::string:
			return serialize_aeon_text_string(*data.str);
		case type::array:
			return serialize_aeon_text_array(*data.ary);
		case type::map:
			return serialize_aeon_text_map(*data.map);
		// can't really make binary fields backwards compatible with json... // TODO -- base64
		case type::binary:
			return serialize_aeon_text_string(data.bin->to_string());
	}
}

std::ostream & operator << (std::ostream & out, asterales::aeon::object const & t) {
	out << t.serialize_text();
	return out;
}

// ================================================================================================
// ------------------------------------------------------------------------------------------------
// ================================================================================================
// PARSE TEXT

typedef std::string::const_iterator sci;

static bool parse_skip_irrelevant(sci & b, sci const & e) {
	for (;b!=e;b++) switch(*b) {
		case '\n':
		case '\r':
		case '\t':
		case ' ':
		case ',':
		case ':':
			continue;
		default:
			return true;
	}
	return false;
}

static aeon::object parse_aeon_text_null(sci & b, sci const & e) {
	if (*b == 'n' &&
		b + 1 != e && *(b + 1) == 'u' &&
		b + 2 != e && *(b + 2) == 'l' &&
		b + 3 != e && *(b + 3) == 'l'
	) {
		b += 4;
		return null_;
	} else throw aeon::exception::parse {};
}


static aeon::object parse_aeon_text_bool(sci & b, sci const & e) {
	if (*b == 't' &&
		b + 1 != e && *(b + 1) == 'r' &&
		b + 2 != e && *(b + 2) == 'u' &&
		b + 3 != e && *(b + 3) == 'e'
	) {
		b += 4;
		return {true};
	} else if (*b == 'f' &&
		b + 1 != e && *(b + 1) == 'a' &&
		b + 2 != e && *(b + 2) == 'l' &&
		b + 3 != e && *(b + 3) == 's' &&
		b + 4 != e && *(b + 4) == 'e'
	) {
		b += 5;
		return {false};
	} else throw aeon::exception::parse {};
}

static aeon::object parse_aeon_text_numerical(sci & b, sci const & e) {
	sci i = b;
	bool is_float = false;
	for (;b!=e;b++) {
		switch(*b) {
			case '.':
			case 'e':
				is_float = true;
				[[fallthrough]];
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
			case '+':
			case '-':
				continue;;
			default:
				break;
		}
		break;
	}
	
	if (i == b) return aeon::null;
	std::string num_str {i, b};
	if (is_float) {
		return aeon::object(strtod(num_str.c_str(), nullptr));
	} else {
		return aeon::object(strtoll(num_str.c_str(), nullptr, 10));
	}
}

static aeon::object parse_aeon_text_string(sci & b, sci const & e) {
	if (*b != '\"') throw aeon::exception::parse {};
	b++;
	std::string str {};
	
	bool is_escaped = false;

	for (;b!=e;b++) {
		switch (*b) {
			case 'b':
				if (is_escaped) {
					is_escaped = false;
					str += '\b';
				} else str += 'b';
				continue;
			case 'f':
				if (is_escaped) {
					is_escaped = false;
					str += '\f';
				} else str += 'f';
				continue;
			case 'n':
				if (is_escaped) {
					is_escaped = false;
					str += '\n';
				} else str += 'n';
				continue;
			case 'r':
				if (is_escaped) {
					is_escaped = false;
					str += '\r';
				} else str += 'r';
				continue;
			case 't':
				if (is_escaped) {
					is_escaped = false;
					str += '\t';
				} else str += 't';
				continue;
			case '\"':
				if (is_escaped) {
					is_escaped = false;
					str += '\"';
				} else break;
				continue;
			case '\\':
				if (is_escaped) {
					is_escaped = false;
					str += '\\';
				} else is_escaped = true;
				continue;;
			default:
				str += *b;
				continue;
		}
		break;
	}
	if (b == e) throw aeon::exception::parse {};
	b++;
	return aeon::object {str};
}

static aeon::object parse_aeon_text_object(sci & b, sci const & e);

static aeon::object parse_aeon_text_array(sci & b, sci const & e) {
	if (*b != '[') throw aeon::exception::parse {};
	b++;
	aeon::object obj {aeon::object::type::array};
	while (b!=e) {
		if (!parse_skip_irrelevant(b, e)) throw aeon::exception::parse {};
		if (*b == ']') {
			b++;
			return obj;
		} else obj.array().push_back(parse_aeon_text_object(b, e));
	}
	throw aeon::exception::parse {};
}

static aeon::object parse_aeon_text_map(sci & b, sci const & e) {
	if (*b != '{') throw aeon::exception::parse {};
	b++;
	aeon::object obj {aeon::object::type::map};
	while (b!=e) {
		if (!parse_skip_irrelevant(b, e)) throw aeon::exception::parse {};
		if (*b == '}') {
			b++;
			return obj;
		}
		aeon::object key = parse_aeon_text_string(b, e);
		if (!parse_skip_irrelevant(b, e)) throw aeon::exception::parse {};
		obj[key.string()] = parse_aeon_text_object(b, e);
	}
	throw aeon::exception::parse {};
}

static aeon::object parse_aeon_text_object(sci & b, sci const & e) {
	aeon::object obj;
	for (;b!=e;b++) switch(*b) {
		case 'n':
			return parse_aeon_text_null(b, e);
		case 't':
		case 'f':
			return parse_aeon_text_bool(b, e);
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
		case '+':
		case '-':
			return parse_aeon_text_numerical(b, e);
		case '\"':
			return parse_aeon_text_string(b, e);
		case '[':
			return parse_aeon_text_array(b, e);
		case '{':
			return parse_aeon_text_map(b, e);
		case '\n':
		case '\r':
		case ' ':
			continue;
		default:
			throw aeon::exception::parse {};
	}
	return obj;
}

aeon::object aeon::object::parse_text(std::string const & str) {
	sci b = str.begin();
	sci e = str.end();
	return parse_aeon_text_object(b, e);
}

// ================================================================================================
// ------------------------------------------------------------------------------------------------
// ================================================================================================
// BINARY COMMON

enum struct binary_type : uint8_t {
	// < 0x80 is number (0 - 127)
	null = 0x80, // 0x80
	boolean_true,
	boolean_false,
	zero,
	one, // 0x84
	int8,
	int16,
	int32,
	int64,
	uint8,
	uint16,
	uint32,
	iuint8,
	iuint16,
	iuint32, // 0x8E
	real32,
	real64, // 0x90
	string,
	string_empty, // 0x92
	array,
	array_empty, // 0x94
	map,
	map_empty, // 0x96
	binary,
	binary_empty, // 0x98
};

struct varuint_header {
	union {
		uint8_t small_value = 0;
		struct {
			uint8_t byte_mask : 7;
			uint8_t encoding : 1;
		} large;
	};
	varuint_header() = default;
	
	void set_value(size_t v) {
		if (v <= 0b01111111) {
			small_value = v;
		} else {
			large.encoding = 1;
			if (v < 0xFF) large.byte_mask = 0;
			else if (v < 0xFFFF) large.byte_mask = 1;
			else if (v < 0xFFFFFF) large.byte_mask = 2;
			else if (v < 0xFFFFFFFF) large.byte_mask = 3;
			else if (v < 0xFFFFFFFFFF) large.byte_mask = 4;
			else if (v < 0xFFFFFFFFFFFF) large.byte_mask = 5;
			else if (v < 0xFFFFFFFFFFFFFF) large.byte_mask = 6;
			else large.byte_mask = 7;
		}
	}
	
	size_t get_num_bytes() {
		if (large.encoding == 0) return 0;
		return large.byte_mask + 1;
	}
};
static_assert (sizeof(varuint_header) == 1);

// ================================================================================================
// ------------------------------------------------------------------------------------------------
// ================================================================================================
// SERIALIZE BINARY

static void serialize_varuint(buffer_assembly & buf, size_t v) {
	varuint_header vh;
	vh.set_value(v);
	buf.write(vh.small_value);
	size_t nb = vh.get_num_bytes();
	if (nb) { buf.write(v, nb); }
}

static void serialize_aeon_binary_integer(buffer_assembly & buf, aeon::int_t i) {
	if (i == 0) {
		buf.write(binary_type::zero);
		return;
	}
	if (i == 1) {
		buf.write(binary_type::one);
		return;
	}
	bool neg = false;
	if (i < 0 && i > -(0xFFFFFFFFL)) { i = -i; neg = true; }
	if (i > 0 && i <= 0xFF) {
		buf.write(neg ? binary_type::iuint8 : binary_type::uint8);
		buf.write<uint8_t>(i);
	} else if (i > 0 && i <= 0xFFFF) {
		buf.write(neg ? binary_type::iuint16 : binary_type::uint16);
		buf.write<uint16_t>(i);
	} else if (i > 0 && i <= 0xFFFFFFFF) {
		buf.write(neg ? binary_type::iuint32 : binary_type::uint32);
		buf.write<uint32_t>(i);
	} else {
		buf.write(binary_type::int64);
		buf.write<int64_t>(i);
	}
}

static void serialize_aeon_binary_string(buffer_assembly & buf, aeon::str_t const & str) {
	if (!str.size()) {
		buf.write(binary_type::string_empty);
	} else {
		buf.write(binary_type::string);
		serialize_varuint(buf, str.size());
		buf.write_many(str.data(), str.size());
	}
}

buffer_assembly aeon::object::serialize_binary() const {
	buffer_assembly buf;
	serialize_binary(buf);
	return buf;
}

void aeon::object::serialize_binary(buffer_assembly & buf) const {
	switch(t_) {
		default:
		case type::none:
			buf.write(binary_type::null);
			return;
		case type::boolean:
			if (data.boolean) buf.write(binary_type::boolean_true); else buf.write(binary_type::boolean_false);
			return;
		case type::integer:
			serialize_aeon_binary_integer(buf, data.num_int);
			return;
		case type::real:
			if (data.num_real == 0) {
				buf.write(binary_type::zero);
			} else {
				buf.write(binary_type::real64);
				buf.write(data.num_real);
			}
			return;
		case type::string:
			serialize_aeon_binary_string(buf, *data.str);
			return;
		case type::array:
			if (!data.ary->size()) {
				buf.write(binary_type::array_empty);
			} else {
				buf.write(binary_type::array);
				serialize_varuint(buf, data.ary->size());
				for (object const & obj : *data.ary) {
					obj.serialize_binary(buf);
				}
			}
			return;
		case type::map:
			if (!data.map->size()) {
				buf.write(binary_type::map_empty);
			} else {
				buf.write(binary_type::map);
				serialize_varuint(buf, data.map->size());
				for (auto const & [key, value] : *data.map) {
					serialize_varuint(buf, key.size());
					buf.write_many(key.data(), key.size());
					value.serialize_binary(buf);
				}
			}
			return;
		case type::binary:
			if (!data.bin->size()) {
				buf.write(binary_type::binary_empty);
			} else {
				buf.write(binary_type::binary);
				serialize_varuint(buf, data.bin->size());
				buf.write_many(data.bin->data(), data.bin->size());
			}
			return;
	}
}

// ================================================================================================
// ------------------------------------------------------------------------------------------------
// ================================================================================================
// PARSE BINARY

static size_t read_varuint(buffer_assembly & buf) {
	pcheck(varuint_header);
	varuint_header vh;
	vh.small_value = buf.read<uint8_t>();
	size_t num_bytes = vh.get_num_bytes();
	if (!num_bytes) return vh.small_value;
	else {
		ncheck(num_bytes);
		return buf.read<size_t>(num_bytes);
	}
	
}

static aeon::str_t parse_aeon_binary_string(buffer_assembly & buf) {
	size_t len = read_varuint(buf);
	std::string str;
	str.resize(len);
	ncheck(len);
	buf.read_many(str.data(), len);
	return str;
}

static aeon::ary_t parse_aeon_binary_array(buffer_assembly & buf) {
	size_t len = read_varuint(buf);
	aeon::ary_t ary;
	for (size_t i = 0; i < len; i++) {
		ary.push_back(aeon::parse_binary(buf));
	}
	return ary;
}

static aeon::map_t parse_aeon_binary_map(buffer_assembly & buf) {
	size_t len = read_varuint(buf);
	aeon::map_t map;
	for (size_t i = 0; i < len; i++) {
		aeon::str_t key = parse_aeon_binary_string(buf);
		map[key] = aeon::parse_binary(buf);
	}
	return map;
}

static aeon::bin_t parse_aeon_binary(buffer_assembly & buf) {
	size_t len = read_varuint(buf);
	aeon::bin_t bin;
	buf.transfer_to(bin, len);
	return bin;
}

template <typename T> inline T ezread(buffer_assembly & buf) {
	pcheck(T);
	return buf.read<T>();
}

aeon::object aeon::object::parse_binary(buffer_assembly & buf) {
	pcheck(binary_type);
	switch (buf.read<binary_type>()) {
		default: pthrow;
		case binary_type::null: return null_;
		case binary_type::boolean_true: return true;
		case binary_type::boolean_false: return false;
		case binary_type::zero: return 0;
		case binary_type::one: return 1;
		case binary_type::int8: return ezread<int8_t>(buf);
		case binary_type::int16: return ezread<int16_t>(buf);
		case binary_type::int32: return ezread<int32_t>(buf);
		case binary_type::int64: return ezread<int64_t>(buf);
		case binary_type::uint8: return ezread<uint8_t>(buf);
		case binary_type::uint16: return ezread<uint16_t>(buf);
		case binary_type::uint32: return ezread<uint32_t>(buf);
		case binary_type::iuint8: return - static_cast<int_t>(ezread<uint8_t>(buf));
		case binary_type::iuint16: return - static_cast<int_t>(ezread<uint16_t>(buf));
		case binary_type::iuint32: return - static_cast<int_t>(ezread<uint32_t>(buf));
		case binary_type::real32: return ezread<float>(buf);
		case binary_type::real64: return ezread<double>(buf);
		case binary_type::string: return parse_aeon_binary_string(buf);
		case binary_type::string_empty: return aeon::string();
		case binary_type::array: return parse_aeon_binary_array(buf);
		case binary_type::array_empty: return aeon::array();
		case binary_type::map: return parse_aeon_binary_map(buf);
		case binary_type::map_empty: return aeon::map();
		case binary_type::binary: return parse_aeon_binary(buf);
		case binary_type::binary_empty: return aeon::binary();
	} 
}

// ================================================================================================
// ------------------------------------------------------------------------------------------------
// ================================================================================================

bool aeon::object::operator == (object const & other) const {
	if (t_ != other.t_) return false;
	switch (t_) {
		case type::none:
			return true;
		case type::boolean:
			return data.boolean == other.data.boolean;
		case type::integer:
			return data.num_int == other.data.num_int;
		case type::real:
			return data.num_real == other.data.num_real;
		case type::string:
			return *data.str == *other.data.str;
		case type::array:
			return *data.ary == *other.data.ary;
		case type::map:
			return *data.map == *other.data.map;
		case type::binary:
			return *data.bin == *other.data.bin;
		default:
			return false;
	}
}
