const ABT = {
	nullean : 0,
	boolean_true : 1,
	boolean_false : 2,
	zero : 3,
	one : 4,
	int8 : 5,
	int16 : 6,
	int32 : 7,
	int64 : 8,
	uint8 : 9,
	uint16 : 10,
	uint32 : 11,
	iuint8 : 12,
	iuint16 : 13,
	iuint32 : 14,
	real32 : 15,
	real64 : 16,
	string : 17,
	string_empty : 18,
	array : 19,
	array_empty : 20,
	map : 21,
	map_empty : 22,
	binary : 23,
	binary_empty : 24,
};

class AEONParser {
	constructor(array) {
		this.ary = array;
		this.cur = array;
	}
	adv(cnt) {
		this.cur = this.cur.subarray(cnt);
	}
	
	parse_varuint() {
		let header = this.cur[0];
		this.adv(1);
		if (header < 0b01111111) {
			return header;
		} else {
			let num_bytes = (header & 0b11100000) >> 5;
			let intbuf = this.cur.subarray(0, num_bytes);
			this.adv(num_bytes);
			return new Uint32Array(intbuf.buffer)[0];
		}
	}
	
	parse_string(par) {
		let size = this.parse_varuint(par);
		let strbuf = this.cur.subarray(0, size);
		this.adv(size);
		return new TextDecoder("utf-8").decode(strbuf);
	}
	
	parse_array(par) {
		let size = this.parse_varuint(par);
		let ary = [];
		for (let i = 0; i < size; i++) {
			ary[i] = this.parse(par);
		}
		return ary;
	}
	
	parse_map(par) {
		let size = this.parse_varuint(par);
		let map = {};
		for (let i = 0; i < size; i++) {
			map[this.parse_string(par)] = this.parse(par);
		}
		return map;
	}
	
	parse_binary(par) {
		let size = this.parse_varuint(par);
		let bin = this.cur.subarray(0, size);
		this.adv(size);
		return bin;
	}
	
	parse(par) {
		let value = null;
		let cc = this.cur[0];
		this.adv(1);
		switch(cc) {
		case ABT.nullean: 
			return null;
			
		case ABT.boolean_true:
			return true;
			
		case ABT.boolean_false: 
			return false;
			
		case ABT.zero:
			return 0;
			
		case ABT.one:
			return 1;
			
		case ABT.int8: 
			value = new Int8Array(this.cur.buffer)[0];
			this.adv(1);
			return value;
			
		case ABT.int16:
			value = new Int16Array(this.cur.buffer)[0];
			this.adv(2);
			return value;
			
		case ABT.int32:
			value = new Int32Array(this.cur.buffer)[0];
			this.adv(4);
			return value;
			
		case ABT.int64: 
			value = new Int32Array(this.cur.buffer)[0];
			this.adv(8);
			return value;
			
		case ABT.uint8:
			value = array[0];
			this.adv(1);
			return value;
			
		case ABT.uint16:
			value = new Uint16Array(this.cur.buffer)[0];
			this.adv(2);
			return value;
			
		case ABT.uint32:
			value = new Uint32Array(this.cur.buffer)[0];
			this.adv(4);
			return value;
			
		case ABT.iuint8:
			this.adv(1);
			value = -array[0];
			return value;
			
		case ABT.iuint16:
			this.adv(2);
			value = -new Uint16Array(this.cur.buffer)[0];
			return value;
			
		case ABT.iuint32:
			this.adv(4);
			value = -new Uint32Array(this.cur.buffer)[0];
			return value;
			
		case ABT.real32:
			this.adv(4);
			value = new Float32Array(this.cur.buffer)[0];
			return value;
			
		case ABT.real64:
			this.adv(8);
			value = new Float64Array(this.cur.buffer)[0];
			return value;
			
		case ABT.string:
			return this.parse_string(par);
			
		case ABT.string_empty:
			return "";
			
		case ABT.array:
			return this.parse_array(par);
			
		case ABT.array_empty:
			return [];
			
		case ABT.map:
			return this.parse_map(par);
			
		case ABT.map_empty:
			return {};
			
		case ABT.binary:
			return this.parse_binary(par);
			
		case ABT.binary_empty:
			return new Uint8Array();
		}
	}
}

class AEONSerializer {
	constructor() {
		this.buffer = new Uint8Array(0);
	}
	append(arr) {
		let new_buffer = new Uint8Array(this.buffer.length + arr.length);
		new_buffer.set(this.buffer, 0);
		new_buffer.set(arr, this.buffer.length);
		this.buffer = new_buffer;
	}
	serialize_ABT(abt) {
		let cc = new Uint8Array(1);
		cc[0] = abt;
		this.append(cc);
	}
	serialize_varuint(size) {
		let vh = new Uint8Array(1);
		if (size < 0b01111111)
			vh[0] = size;
		else {
			// TODO
		}
		this.append(vh);
	}
	serialize_string(str) {
		this.serialize_varuint(str.length)
		let nb = new TextEncoder("utf-8").encode(str);
		this.append(nb);
	}
	serialize(obj) {
		let nb = null;
		switch (typeof obj) {
		default:
			this.serialize_ABT(ABT.nullean);
			return;
		case 'boolean':
			this.serialize_ABT(obj ? ABT.boolean_true : ABT.boolean_false);
			return;
		case 'number':
			if (obj % 1 === 0) {
				this.serialize_ABT(ABT.int32);
				let intary = new Int32Array(1); // TODO moar ABTs
				intary[0] = obj;
				nb = new Uint8Array(intary.buffer);
				this.append(nb);
				return;
			} else {
				this.serialize_ABT(ABT.real64);
				let flary = new Float64Array(1);
				flary[0] = obj;
				nb = new Uint8Array(flary.buffer);
				this.append(nb);
				return;
			}
		case 'string':
			this.serialize_ABT(ABT.string);
			this.serialize_string(obj)
			return;
		case 'object':
			if (obj instanceof Uint8Array) {
				this.serialize_ABT(ABT.binary);
				this.serialize_varuint(obj.length);
				this.append(obj)
			}
			else if (Array.isArray(obj)) {
				this.serialize_ABT(ABT.array);
				this.serialize_varuint(obj.length)
				obj.forEach((i)=>{this.serialize(i)});
			} else if (obj === Object(obj)) {
				this.serialize_ABT(ABT.map);
				this.serialize_varuint(Object.keys(obj).length)
				for (let key in obj) {
					this.serialize_string(key);
					this.serialize(obj[key]);
				}
			} else {
				this.serialize_ABT(ABT.nullean);
			}
		}
	}
}

class AEON {
	
	static POST(url, object, response_cb) {
		let request = new XMLHttpRequest()
		request.open('POST', url, true)
		request.responseType = 'arraybuffer';
		request.setRequestHeader("Content-Type", "application/aeon")
		request.onload = function() {
			
			let buffer = new Uint8Array(request.response);
			let obj = AEON.parse(buffer);
			response_cb(obj);
		}
		request.send(AEON.serialize(object));
	}
	
	static parse(buffer) {
		return new AEONParser(buffer).parse();
	}
	
	static serialize(object) {
		let s = new AEONSerializer();
		s.serialize(object);
		return s.buffer;
	}
}
