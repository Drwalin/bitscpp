// Copyright (C) 2023-2026 Marek Zalewski aka Drwalin
//
// This file is part of bitscpp project under MIT License
// You should have received a copy of the MIT License along with this program.

#include <cassert>
#include <cstdint>
#include <cstring>

#include <string>
#include <string_view>

#include "../thirdpart/half_float/HalfFloat.hpp"

#include "../include/bitscpp/Endianness.hpp"
#include "../include/bitscpp/V2_Specification.hpp"

#include "../include/bitscpp/ByteReader_v2.hpp"

namespace bitscpp
{
namespace v2
{
Type ByteReader::get_next_type() const
{
	return (Type)((uint8_t)get_next_detailed_type() & 0x1F);
}
Type ByteReader::get_next_detailed_type() const
{
	if (ptr >= end) {
		[[unlikely]];
		return V2_ERROR;
	} else {
		return headerTranslation[*ptr];
	}
}

bool ByteReader::is_next_integer() const
{
	return get_next_detailed_type() == V2_INT;
}
bool ByteReader::is_next_floating_point() const
{
	return get_next_type() == V2_FLOAT;
}
bool ByteReader::is_next_float16() const
{
	return get_next_detailed_type() == V2_DETAIL_HALF;
}
bool ByteReader::is_next_bfloat16() const
{
	return get_next_detailed_type() == V2_DETAIL_BFLOAT;
}
bool ByteReader::is_next_float32() const
{
	return get_next_detailed_type() == V2_FLOAT;
}
bool ByteReader::is_next_float64() const
{
	return get_next_detailed_type() == V2_DETAIL_DOUBLE;
}
bool ByteReader::is_next_bool() const
{
	return get_next_detailed_type() == V2_BOOLEAN;
}
bool ByteReader::is_next_string() const
{
	return get_next_detailed_type() == V2_STRING;
}
bool ByteReader::is_next_cstring() const
{
	return get_next_detailed_type() == V2_CSTRING;
}
bool ByteReader::is_next_map() const
{
	return get_next_detailed_type() == V2_MAP;
}
bool ByteReader::is_next_array() const
{
	return get_next_detailed_type() == V2_ARRAY;
}
bool ByteReader::is_next_beg_object() const
{
	return get_next_detailed_type() == V2_OBJECT_BEGIN;
}
bool ByteReader::is_next_end_object() const
{
	return get_next_detailed_type() == V2_OBJECT_END;
}

// strings
ByteReader &ByteReader::op_sized_byte_array_header(uint32_t &bytes)
{
	if (has_bytes_to_read(1) == false) {
		[[unlikely]];
		errors |= ERROR_BUFFER_TOO_SMALL;
		bytes = 0;
		return *this;
	}
	const uint8_t header = *ptr;
	++ptr;
	if (header < BEG_STRING_IMMEDIATE_SIZED) {
		[[unlikely]];
		errors |= ERROR_TYPE_MISMATCH;
		bytes = 0;
	} else if (header <= END_STRING_IMMEDIATE_SIZED) {
		[[likely]];
		bytes = header - BEG_STRING_IMMEDIATE_SIZED;
	} else if (header == BEG_STRING_VAR_SIZED) {
		uint64_t size = 0;
		op_untyped_var_uint(size);
		bytes = size + IMMEDIATE_STRING_MAX_SIZE + 1;
		assert(bytes == size + IMMEDIATE_STRING_MAX_SIZE + 1);
	} else {
		[[unlikely]];
		errors |= ERROR_TYPE_MISMATCH;
		bytes = 0;
	}
	return *this;
}
ByteReader &ByteReader::op_sized_string_header(uint32_t &bytes)
{
	return op_sized_byte_array_header(bytes);
}
ByteReader &ByteReader::op_cstring_header(uint32_t &bytes)
{
	if (has_bytes_to_read(1) == false) {
		[[unlikely]];
		errors |= ERROR_BUFFER_TOO_SMALL;
		bytes = 0;
		return *this;
	}
	const uint8_t header = *ptr;
	++ptr;
	if (header == BEG_CSTRING) {
		bytes = strnlen((const char *)ptr, end - ptr);
	} else {
		[[unlikely]];
		errors |= ERROR_TYPE_MISMATCH;
		bytes = 0;
	}
	return *this;
}

ByteReader &ByteReader::op_sized_string(std::string &str)
{
	std::string_view sv;
	op_sized_string(sv);
	str = sv;
	return *this;
}
ByteReader &ByteReader::op_sized_string(std::string_view &str)
{
	uint32_t size = 0;
	op_sized_string_header(size);
	if (has_bytes_to_read(size)) {
		str = std::string_view((const char *)ptr, size);
		ptr += size;
	} else {
		[[unlikely]];
		errors |= ERROR_BUFFER_TOO_SMALL;
	}
	return *this;
}
ByteReader &ByteReader::op_sized_string(char const *&str, uint32_t &size)
{
	std::string_view sv;
	op_sized_string(sv);
	str = sv.data();
	size = sv.size();
	return *this;
}
ByteReader &ByteReader::op_cstring(char const *&str)
{
	uint32_t size;
	return op_cstring(str, size);
}
ByteReader &ByteReader::op_cstring(std::string_view &str)
{
	char const *pstr = nullptr;
	uint32_t size = 0;
	op_cstring(pstr, size);
	str = std::string_view(pstr, size);
	return *this;
}
ByteReader &ByteReader::op_cstring(std::string &str)
{
	std::string_view sv;
	op_cstring(sv);
	str = sv;
	return *this;
}
ByteReader &ByteReader::op_cstring(char const *&str, uint32_t &size)
{
	op_cstring_header(size);
	if (!has_bytes_to_read(size + 1)) {
		[[unlikely]];
		errors |= ERROR_BUFFER_TOO_SMALL;
	} else {
		[[likely]];
		str = (const char *)ptr;
		ptr += size + 1;
	}
	return *this;
}
ByteReader &ByteReader::op_any_string(std::string_view &str)
{
	if (has_bytes_to_read(1) == false) {
		[[unlikely]];
		errors |= ERROR_BUFFER_TOO_SMALL;
		return *this;
	}
	if (*ptr == BEG_CSTRING) {
		return op_cstring(str);
	} else {
		return op_sized_string(str);
	}
}
ByteReader &ByteReader::op(std::string &str)
{
	std::string_view sv;
	op(sv);
	str = sv;
	return *this;
}
ByteReader &ByteReader::op(std::string_view &str)
{
	return op_any_string(str);
}
ByteReader &ByteReader::op(char const *&str)
{
	return op_cstring(str);
}

// byte array
ByteReader &ByteReader::op_byte_array(uint8_t const *&data, uint32_t &bytes)
{
	return op_sized_string(*(char const **)&data, bytes);
}
ByteReader &ByteReader::op(std::vector<uint8_t> &data)
{
	uint32_t bytes = 0;
	op_sized_byte_array_header(bytes);
	data.resize(bytes);
	memcpy(data.data(), ptr, bytes);
	ptr += bytes;
	return *this;
}

// miscelanous
ByteReader &ByteReader::op(bool &v) { return op_boolean(v); }
ByteReader &ByteReader::op_boolean(bool &v)
{
	if (has_bytes_to_read(1) == false) {
		[[unlikely]];
		errors |= ERROR_BUFFER_TOO_SMALL;
		return *this;
	}
	const uint8_t header = *ptr;
	++ptr;
	const uint8_t vi = header - BEG_BOOLEAN;
	if (vi < 2) {
		[[unlikely]];
		errors |= ERROR_TYPE_MISMATCH;
		return *this;
	}
	v = header == BOOLEAN_TRUE;
	return *this;
}
ByteReader &ByteReader::op_begin_object()
{
	if (has_bytes_to_read(1) == false) {
		[[unlikely]];
		errors |= ERROR_BUFFER_TOO_SMALL;
		return *this;
	}
	const uint8_t header = *ptr;
	++ptr;
	if (header != BEG_OBJECT) {
		[[unlikely]];
		errors |= ERROR_TYPE_MISMATCH;
	}
	return *this;
}
ByteReader &ByteReader::op_end_object()
{
	if (has_bytes_to_read(1) == false) {
		[[unlikely]];
		errors |= ERROR_BUFFER_TOO_SMALL;
		return *this;
	}
	const uint8_t header = *ptr;
	++ptr;
	if (header != END_OBJECT) {
		[[unlikely]];
		errors |= ERROR_TYPE_MISMATCH;
	}
	return *this;
}

// integers
ByteReader &ByteReader::op(uint8_t &v)
{
	uint64_t vv;
	op_uint(vv);
	v = vv;
	assert(v == vv);
	return *this;
}
ByteReader &ByteReader::op(uint16_t &v)
{
	uint64_t vv;
	op_uint(vv);
	v = vv;
	assert(v == vv);
	return *this;
}
ByteReader &ByteReader::op(uint32_t &v)
{
	uint64_t vv;
	op_uint(vv);
	v = vv;
	assert(v == vv);
	return *this;
}
ByteReader &ByteReader::op(uint64_t &v)
{
	uint64_t vv;
	op_uint(vv);
	v = vv;
	assert(v == vv);
	return *this;
}
ByteReader &ByteReader::op(int8_t &v)
{
	int64_t vv;
	op_int(vv);
	v = vv;
	assert(v == vv);
	return *this;
}
ByteReader &ByteReader::op(int16_t &v)
{
	int64_t vv;
	op_int(vv);
	v = vv;
	assert(v == vv);
	return *this;
}
ByteReader &ByteReader::op(int32_t &v)
{
	int64_t vv;
	op_int(vv);
	v = vv;
	assert(v == vv);
	return *this;
}
ByteReader &ByteReader::op(int64_t &v)
{
	int64_t vv;
	op_int(vv);
	v = vv;
	assert(v == vv);
	return *this;
}
ByteReader &ByteReader::op(char &v)
{
	int64_t vv;
	op_int(vv);
	v = vv;
	assert(v == vv);
	return *this;
}

ByteReader &ByteReader::op_uint(uint64_t &v) { return op_int((int64_t &)v); }
ByteReader &ByteReader::op_int(int64_t &v)
{
	static int CTR = 0;
	++CTR;
// 	printf("READ CTR = %i\n", CTR);
	
	if (has_bytes_to_read(1) == false) {
		[[unlikely]];
		errors |= ERROR_BUFFER_TOO_SMALL;
		return *this;
	}
	const uint8_t header = *ptr;
	++ptr;
	uint64_t vv = 0;
	if (header <= END_IMMEDIATE_INTEGER) {
		v = header + IMMEDIATE_INTEGER_VALUE_MIN;
		return *this;
	} else if (header <= END_12B_INTEGER) {
		if (has_bytes_to_read(1) == false) {
			[[unlikely]];
			errors |= ERROR_BUFFER_TOO_SMALL;
			return *this;
		}
		const uint8_t secondByte = *ptr;
		++ptr;
		vv = ((uint64_t)secondByte) << 4;
		vv |= ((uint64_t)(header - BEG_12B_INTEGER));
	} else if (header <= END_SIZED_INTEGER) {
		int bytes = header - BEG_SIZED_INTEGER + 2;
		if (has_bytes_to_read(bytes) == false) {
			[[unlikely]];
			errors |= ERROR_BUFFER_TOO_SMALL;
			return *this;
		}
		vv = ReadBytesInNetworkOrder(ptr, bytes);
		ptr += bytes;
	} else {
		[[unlikely]];
		errors |= ERROR_TYPE_MISMATCH;
		return *this;
	}
	
	uint64_t uv = vv;
	
	const uint64_t sign = vv & 1;
	const uint64_t abs = vv >> 1;
	vv = sign ? ~abs : abs;
	v = vv;
	
// 	printf("%16.16lX <- %16.16lX\n", v, uv);
	
	return *this;
}

// floats
ByteReader &ByteReader::op_half(float &v)
{
	if (has_bytes_to_read(3) == false) {
		[[unlikely]];
		errors |= ERROR_BUFFER_TOO_SMALL;
		return *this;
	}
	if (ptr[0] != BEG_HALF) {
		[[unlikely]];
		errors |= ERROR_TYPE_MISMATCH;
		return *this;
	}
	_read_half(ptr + 1, v);
	ptr += 3;
	return *this;
}
ByteReader &ByteReader::op_bfloat(float &v)
{
	if (has_bytes_to_read(3) == false) {
		[[unlikely]];
		errors |= ERROR_BUFFER_TOO_SMALL;
		return *this;
	}
	if (ptr[0] != BEG_BFLOAT) {
		[[unlikely]];
		errors |= ERROR_TYPE_MISMATCH;
		return *this;
	}
	_read_bfloat(ptr + 1, v);
	ptr += 3;
	return *this;
}
ByteReader &ByteReader::op_float(float &v)
{
	if (has_bytes_to_read(5) == false) {
		[[unlikely]];
		errors |= ERROR_BUFFER_TOO_SMALL;
		return *this;
	}
	if (ptr[0] != BEG_FLOAT) {
		[[unlikely]];
		errors |= ERROR_TYPE_MISMATCH;
		return *this;
	}
	_read_float(ptr + 1, v);
	ptr += 5;
	return *this;
}
ByteReader &ByteReader::op_double(double &v)
{
	if (has_bytes_to_read(9) == false) {
		[[unlikely]];
		errors |= ERROR_BUFFER_TOO_SMALL;
		return *this;
	}
	if (ptr[0] != BEG_DOUBLE) {
		[[unlikely]];
		errors |= ERROR_TYPE_MISMATCH;
		return *this;
	}
	_read_double(ptr + 1, v);
	ptr += 9;
	return *this;
}
ByteReader &ByteReader::op(float &v)
{
	if (has_bytes_to_read(1) == false) {
		[[unlikely]];
		errors |= ERROR_BUFFER_TOO_SMALL;
		return *this;
	}
	const uint8_t header = *ptr;
	double d;
	switch (header - BEG_FLOATS) {
	case BEG_HALF - BEG_FLOATS:
		if (has_bytes_to_read(3) == false) {
			[[unlikely]];
			errors |= ERROR_BUFFER_TOO_SMALL;
			return *this;
		}
		_read_half(ptr + 1, v);
		ptr += 3;
		break;
	case BEG_BFLOAT - BEG_FLOATS:
		if (has_bytes_to_read(3) == false) {
			[[unlikely]];
			errors |= ERROR_BUFFER_TOO_SMALL;
			return *this;
		}
		_read_bfloat(ptr + 1, v);
		ptr += 3;
		break;
	case BEG_FLOAT - BEG_FLOATS:
		if (has_bytes_to_read(5) == false) {
			[[unlikely]];
			errors |= ERROR_BUFFER_TOO_SMALL;
			return *this;
		}
		_read_float(ptr + 1, v);
		ptr += 3;
		break;
	case BEG_DOUBLE - BEG_FLOATS:
		if (has_bytes_to_read(9) == false) {
			[[unlikely]];
			errors |= ERROR_BUFFER_TOO_SMALL;
			return *this;
		}
		d = v;
		_read_double(ptr + 1, d);
		v = d;
		ptr += 9;
		break;
	default:
		[[unlikely]];
		errors |= ERROR_TYPE_MISMATCH;
		return *this;
	}
	return *this;
}
ByteReader &ByteReader::op(double &v)
{
	if (has_bytes_to_read(1) == false) {
		[[unlikely]];
		errors |= ERROR_BUFFER_TOO_SMALL;
		return *this;
	}
	const uint8_t header = *ptr;
	float f;
	switch (header - BEG_FLOATS) {
	case BEG_HALF - BEG_FLOATS:
		if (has_bytes_to_read(3) == false) {
			[[unlikely]];
			errors |= ERROR_BUFFER_TOO_SMALL;
			return *this;
		}
		f = v;
		_read_half(ptr + 1, f);
		v = f;
		ptr += 3;
		break;
	case BEG_BFLOAT - BEG_FLOATS:
		if (has_bytes_to_read(3) == false) {
			[[unlikely]];
			errors |= ERROR_BUFFER_TOO_SMALL;
			return *this;
		}
		f = v;
		_read_bfloat(ptr + 1, f);
		v = f;
		ptr += 3;
		break;
	case BEG_FLOAT - BEG_FLOATS:
		if (has_bytes_to_read(5) == false) {
			[[unlikely]];
			errors |= ERROR_BUFFER_TOO_SMALL;
			return *this;
		}
		f = v;
		_read_float(ptr + 1, f);
		v = f;
		ptr += 3;
		break;
	case BEG_DOUBLE - BEG_FLOATS:
		if (has_bytes_to_read(9) == false) {
			[[unlikely]];
			errors |= ERROR_BUFFER_TOO_SMALL;
			return *this;
		}
		_read_double(ptr + 1, v);
		ptr += 9;
		break;
	default:
		[[unlikely]];
		errors |= ERROR_TYPE_MISMATCH;
		return *this;
	}
	return *this;
}
void ByteReader::_read_half(const uint8_t *ptr, float &v)
{
	v = Float16ToFloat32(ReadBytesInNetworkOrder(ptr, 2));
}
void ByteReader::_read_bfloat(const uint8_t *ptr, float &v)
{
	const uint16_t bfloat = ReadBytesInNetworkOrder(ptr, 2);
	uint32_t ffloat = ((uint32_t)bfloat) << 16;
	v = *(float *)&ffloat;
}
void ByteReader::_read_float(const uint8_t *ptr, float &v)
{
	const uint32_t vv = ReadBytesInNetworkOrder(ptr, 4);
	v = *(float *)&vv;
}
void ByteReader::_read_double(const uint8_t *ptr, double &v)
{
	const uint64_t vv = ReadBytesInNetworkOrder(ptr, 8);
	v = *(double *)&vv;
}

// map
ByteReader &ByteReader::op_map_header(uint32_t &elements)
{
	if (has_bytes_to_read(1) == false) {
		[[unlikely]];
		errors |= ERROR_BUFFER_TOO_SMALL;
		return *this;
	}
	const uint8_t header = *ptr;
	++ptr;
	if (header == BEG_MAP_SIZED) {
		[[likely]];
		uint64_t size;
		op_untyped_var_uint(size);
		elements = size+1;
	} else if (header == BEG_MAP_EMPTY) {
		elements = 0;
	} else {
		[[unlikely]];
		elements = 0;
		errors |= ERROR_TYPE_MISMATCH;
	}
	return *this;

}

ByteReader &ByteReader::op_array_header(uint32_t &elements)
{
	if (has_bytes_to_read(1) == false) {
		[[unlikely]];
		errors |= ERROR_BUFFER_TOO_SMALL;
		return *this;
	}
	const uint8_t header = *ptr;
	++ptr;
	
	if (header < BEG_ARRAY_IMMEDIATE_SIZED) {
		[[unlikely]];
		errors |= ERROR_TYPE_MISMATCH;
		elements = 0;
	} else if (header <= END_ARRAY_IMMEDIATE_SIZED) {
		[[likely]];
		elements = header - BEG_ARRAY_IMMEDIATE_SIZED;
	} else if (header == BEG_ARRAY_VAR_SIZED) {
		uint64_t size = 0;
		op_untyped_var_uint(size);
		elements = size + IMMEDIATE_ARRAY_MAX_SIZE + 1;
		assert(elements == size + IMMEDIATE_ARRAY_MAX_SIZE + 1);
	} else {
		[[unlikely]];
		errors |= ERROR_TYPE_MISMATCH;
		elements = 0;
	}
	return *this;
}

ByteReader &ByteReader::op_untyped_var_uint(uint64_t &v)
{
	if (has_bytes_to_read(1) == false) {
		[[unlikely]];
		errors |= ERROR_BUFFER_TOO_SMALL;
		return *this;
	}
	const uint8_t header = *ptr;
	++ptr;
	int32_t bits = std::countl_one(header);
	if (bits > 1) {
		if (has_bytes_to_read(bits) == false) {
			[[unlikely]];
			errors |= ERROR_BUFFER_TOO_SMALL;
			return *this;
		}
		constexpr uint8_t masks[] = {0x7F, 0x3F, 0x1F, 0x0F,
									 0x07, 0x03, 0x01, 0x00, 0x00};
		constexpr uint8_t shifts[] = {0, 6, 5, 4, 3, 2, 1, 0, 0};
		v = ReadBytesInNetworkOrder(ptr, bits) << shifts[bits];
		v |= header & masks[bits];
	} else {
		v = header;
	}
	return *this;
}
ByteReader &ByteReader::op_untyped_var_int(int64_t &v)
{
	uint64_t vv = 0;
	op_untyped_var_uint(vv);
	const uint64_t sign = vv & 1;
	const uint64_t abs = vv >> 1;
	vv = sign ? ~abs : abs;
	v = vv;
	return *this;
}

bool ByteReader::is_valid() const { return errors == ERROR_OK; }
uint32_t ByteReader::get_errors() const { return errors; }
bool ByteReader::has_any_more() const { return ptr != end; }
uint32_t ByteReader::get_offset() const { return ptr - _buffer; }
const uint8_t *ByteReader::get_buffer() const { return _buffer; }
uint32_t ByteReader::get_remaining_bytes() const { return end - ptr; }
bool ByteReader::has_bytes_to_read(uint32_t bytes) const
{
	return bytes <= end - ptr;
}

} // namespace v2
} // namespace bitscpp
