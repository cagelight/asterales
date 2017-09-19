#include "asterid/json.hh"

#include <iomanip>
#include <sstream>
#include <limits>

using namespace asterid;

json::object null_ {};
json::object const & json::null {null_};

//================================================================================================
//------------------------------------------------------------------------------------------------
//================================================================================================

json::object::object(type t) : t_(t) {
	switch(t_) {
		case type::none: break;
		case type::boolean: data.boolean = false; break;
		case type::integer: data.num_int = 0; break;
		case type::real: data.num_real = 0; break;
		case type::string: data.str = new str_t {}; break;
		case type::array: data.ary = new ary_t {}; break;
		case type::map: data.map = new map_t {}; break;
	}
}

json::object::~object() {
	switch(t_) {
		case type::none:
		case type::boolean:
		case type::integer:
		case type::real: break;
		case type::string: delete data.str; break;
		case type::array: delete data.ary; break;
		case type::map: delete data.map; break;
	}
}

json::object::object(bool v) : t_(type::boolean) {
	data.boolean = v;
}

json::object::object(int_t v) : t_(type::integer) {
	data.num_int = v;
}

json::object::object(real_t v) : t_(type::real) {
	data.num_real = v;
}

json::object::object(str_t const & v) : t_(type::string) {
	data.str = new str_t(v);
}

json::object::object(str_t && v) : t_(type::string) {
	data.str = new str_t(std::forward<str_t &&>(v));
}

json::object::object(ary_t const & v) : t_(type::array) {
	data.ary = new ary_t(v);
}

json::object::object(ary_t && v) : t_(type::array) {
	data.ary = new ary_t(std::forward<ary_t &&>(v));
}

json::object::object(map_t const & v) : t_(type::map) {
	data.map = new map_t(v);
}

json::object::object(map_t && v) : t_(type::map) {
	data.map = new map_t(std::forward<map_t &&>(v));
}

json::object::object(object const & other) : t_(other.t_) {
	switch(t_) {
		case type::none: break;
		case type::boolean: data.boolean = other.data.boolean; break;
		case type::integer: data.num_int = other.data.num_int; break;
		case type::real: data.num_real = other.data.num_real; break;
		case type::string: data.str = new str_t (*other.data.str); break;
		case type::array: data.ary = new ary_t (*other.data.ary); break;
		case type::map: data.map = new map_t (*other.data.map); break;
	}
}

json::object::object(object && other) : t_(other.t_) {
	data = other.data;
	other.t_ = type::none;
}

//================================================================================================
//------------------------------------------------------------------------------------------------
//================================================================================================

json::object & json::object::operator = (object const & other) {
	switch(t_) {
		case type::none: break;
		case type::boolean: data.boolean = other.data.boolean; break;
		case type::integer: data.num_int = other.data.num_int; break;
		case type::real: data.num_real = other.data.num_real; break;
		case type::string: data.str = new str_t (*other.data.str); break;
		case type::array: data.ary = new ary_t (*other.data.ary); break;
		case type::map: data.map = new map_t (*other.data.map); break;
	}
	return *this;
}

json::object & json::object::operator = (object && other) {
	t_ = other.t_;
	data = other.data;
	other.t_ = type::none;
	return *this;
}

//================================================================================================
//------------------------------------------------------------------------------------------------
//================================================================================================

bool & json::object::boolean() {
	if (t_ != type::boolean) *this = object {type::boolean};
	return data.boolean;
}

static bool constexpr null_bool = 0;
bool const & json::object::boolean() const {
	if (t_ != type::boolean) return null_bool;
	return data.boolean;
}

json::int_t & json::object::integer() {
	if (t_ != type::integer) *this = object {type::integer};
	return data.num_int;
}

static json::int_t constexpr null_int = 0;
json::int_t const & json::object::integer() const {
	if (t_ != type::integer) return null_int;
	return data.num_int;
}

json::real_t & json::object::real() {
	if (t_ != type::real) *this = object {type::real};
	return data.num_real;
}

static json::real_t constexpr null_real = 0;
json::real_t const & json::object::real() const {
	if (t_ != type::real) return null_real;
	return data.num_real;
}

json::str_t & json::object::string() {
	if (t_ != type::string) *this = object {type::string};
	return *data.str;
}

static json::str_t const null_string {};
json::str_t const & json::object::string() const {
	if (t_ != type::string) return null_string;
	return *data.str;
}

json::ary_t & json::object::array() {
	if (t_ != type::array) *this = object {type::array};
	return *data.ary;
}

static json::ary_t const null_array {};
json::ary_t const & json::object::array() const {
	if (t_ != type::array) return null_array;
	return *data.ary;
}

json::map_t & json::object::map() {
	if (t_ != type::map) *this = object {type::map};
	return *data.map;
}

static json::map_t const null_map {};
json::map_t const & json::object::map() const {
	if (t_ != type::map) return null_map;
	return *data.map;
}

//================================================================================================
//------------------------------------------------------------------------------------------------
//================================================================================================

json::object & json::object::operator [] (size_t i) {
	if (t_ != type::array) *this = object {type::array};
	if (data.ary->size() <= i) data.ary->resize(i + 1);
	return data.ary->at(i);
}

json::object const & json::object::operator [] (size_t i) const {
	if (t_ != type::array) return null;
	if (data.ary->size() <= i) return null;
	return data.ary->at(i);
}

json::object & json::object::operator [] (json::str_t const & key) {
	if (t_ != type::map) *this = object {type::map};
	return data.map->operator[](key);
}

json::object const & json::object::operator [] (json::str_t const & key) const {
	if (t_ != type::map) return null;
	auto const & i = data.map->find(key);
	if (i == data.map->end()) return null;
	return i->second;
}

//================================================================================================
//------------------------------------------------------------------------------------------------
//================================================================================================

static std::string serialize_real(json::real_t v) {
	std::ostringstream r {};
	r.precision(std::numeric_limits<json::real_t>::max_digits10);
	r << v;
	return r.str();
}

static std::string serialize_string(json::str_t const & v) {
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

static std::string serialize_array(json::ary_t const & v) {
	std::ostringstream r {};
	r << '[';
	bool first = true;
	for (auto const & i : v) {
		if (!first) r << ", "; else first = false;
		r << i;
	}
	r << ']';
	return r.str();
}

static std::string serialize_map(json::map_t const & v) {
	std::ostringstream r {};
	r << '{';
	bool first = true;
	for (auto const & i : v) {
		if (!first) r << ", "; else first = false;
		r << serialize_string(i.first);
		r << ": ";
		r << i.second;
	}
	r << '}';
	return r.str();
}

std::string json::object::serialize() const {
	switch(t_) {
		default:
		case type::none:
			return "null";
		case type::boolean:
			return data.boolean ? "true" : "false";
		case type::integer:
			return std::to_string(data.num_int);
		case type::real:
			return serialize_real(data.num_real);
		case type::string:
			return serialize_string(*data.str);
		case type::array:
			return serialize_array(*data.ary);
		case type::map:
			return serialize_map(*data.map);
	}
}

std::ostream & operator << (std::ostream & out, asterid::json::object const & t) {
	out << t.serialize();
	return out;
}

//================================================================================================
//------------------------------------------------------------------------------------------------
//================================================================================================

typedef std::string::const_iterator sci;

static bool parse_skip_irrelevant(sci & b, sci const & e) {
	for (;b!=e;b++) switch(*b) {
		case '\n':
		case '\r':
		case ' ':
		case ',':
		case ':':
			continue;
		default:
			return true;
	}
	return false;
}

static json::object parse_json_bool(sci & b, sci const & e) {
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
	} else throw json::exception::parse {};
}

static json::object parse_json_numerical(sci & b, sci const & e) {
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
	
	if (i == b) return json::null;
	std::string num_str {i, b};
	if (is_float) {
		return json::object(strtod(num_str.c_str(), nullptr));
	} else {
		return json::object(strtoll(num_str.c_str(), nullptr, 10));
	}
}

static json::object parse_json_string(sci & b, sci const & e) {
	if (*b != '\"') throw json::exception::parse {};
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
	if (b == e) throw json::exception::parse {};
	b++;
	return json::object {str};
}

static json::object parse_json_object(sci & b, sci const & e);

static json::object parse_json_array(sci & b, sci const & e) {
	if (*b != '[') throw json::exception::parse {};
	b++;
	json::object obj {json::object::type::array};
	while (b!=e) {
		if (!parse_skip_irrelevant(b, e)) throw json::exception::parse {};
		if (*b == ']') {
			b++;
			return obj;
		} else obj.array().push_back(parse_json_object(b, e));
	}
	throw json::exception::parse {};
}

static json::object parse_json_map(sci & b, sci const & e) {
	if (*b != '{') throw json::exception::parse {};
	b++;
	json::object obj {json::object::type::map};
	while (b!=e) {
		if (!parse_skip_irrelevant(b, e)) throw json::exception::parse {};
		if (*b == '}') {
			b++;
			return obj;
		}
		json::object key = parse_json_string(b, e);
		if (!parse_skip_irrelevant(b, e)) throw json::exception::parse {};
		obj[key.string()] = parse_json_object(b, e);
	}
	throw json::exception::parse {};
}

static json::object parse_json_object(sci & b, sci const & e) {
	json::object obj;
	for (;b!=e;b++) switch(*b) {
		case 't':
		case 'f':
			return parse_json_bool(b, e);
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
			return parse_json_numerical(b, e);
		case '\"':
			return parse_json_string(b, e);
		case '[':
			return parse_json_array(b, e);
		case '{':
			return parse_json_map(b, e);
		case '\n':
		case '\r':
		case ' ':
			continue;
		default:
			throw json::exception::parse {};
	}
	return obj;
}

json::object json::object::parse(std::string const & str) {
	sci b = str.begin();
	sci e = str.end();
	return parse_json_object(b, e);
}

//================================================================================================
//------------------------------------------------------------------------------------------------
//================================================================================================

bool json::object::operator == (object const & other) {
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
		default:
			return false;
	}
}
