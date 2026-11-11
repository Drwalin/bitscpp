// Copyright (C) 2023-2026 Marek Zalewski aka Drwalin
//
// This file is part of bitscpp project under MIT License
// You should have received a copy of the MIT License along with this program.

#ifdef BITSCPP_BYTE_WRITER_V2_2_HPP
#pragma once
#endif

#ifndef BITSCPP_BYTE_READER_V2_CPP
#define BITSCPP_BYTE_READER_V2_CPP

#if not defined _BITSCPP_INLINE
#define BITSCPP_NO_INLINE_CPP_BYTE_READER_V2
#endif

#include <cassert>
#include <cstdint>

#include <string>
#include <string_view>

#include "../thirdparty/half_float/HalfFloat.hpp"

#include "../include/bitscpp/Endianness.hpp"
#include "../include/bitscpp/V2_Specification.hpp"

#include "../include/bitscpp/ByteReader_v2.hpp"

#undef BITSCPP_INLINE
#if defined BITSCPP_NO_INLINE_CPP_BYTE_READER_V2
#define BITSCPP_INLINE
#else
#define BITSCPP_INLINE _BITSCPP_INLINE
#endif

namespace bitscpp
{
namespace v2
{
BITSCPP_INLINE ByteReader::ByteReader(const uint8_t *buffer, uint32_t offset,
									  uint32_t size)
	: _buffer(buffer), ptr(buffer ? buffer + offset : nullptr),
	  end(buffer ? buffer + size : nullptr), total_size(buffer ? size : 0),
	  errors(0)
{
	if (buffer == nullptr) {
		ptr = nullptr;
		end = nullptr;
		set_error(ERROR_BUFFER_NULLPTR);
	}
}

BITSCPP_INLINE ByteReader::ByteReader(const uint8_t *buffer, uint32_t size)
	: ByteReader(buffer, 0, size)
{
}

BITSCPP_INLINE ByteReader::ByteReader(const ByteReader &o)
{
	_buffer = o._buffer;
	ptr = o.ptr;
	end = o.end;
	total_size = o.total_size;
	errors = o.errors;
}

BITSCPP_INLINE ByteReader::ByteReader(ByteReader &o)
{
	_buffer = o._buffer;
	ptr = o.ptr;
	end = o.end;
	total_size = o.total_size;
	errors = o.errors;
}

BITSCPP_INLINE ByteReader::ByteReader(ByteReader &&o)
{
	_buffer = o._buffer;
	ptr = o.ptr;
	end = o.end;
	total_size = o.total_size;
	errors = o.errors;
	o._buffer = nullptr;
	o.ptr = nullptr;
	o.end = nullptr;
	o.total_size = 0;
	o.errors = 0;
}

BITSCPP_INLINE ByteReader &ByteReader::operator=(const ByteReader &o)
{
	return *new (this) ByteReader(o);
}

BITSCPP_INLINE ByteReader &ByteReader::operator=(ByteReader &o)
{
	return *new (this) ByteReader(o);
}

BITSCPP_INLINE ByteReader &ByteReader::operator=(ByteReader &&o)
{
	return *new (this) ByteReader(std::move(o));
}

BITSCPP_INLINE Type ByteReader::get_next_type() const
{
	return (Type)((uint8_t)get_next_detailed_type() & 0x1F);
}
BITSCPP_INLINE Type ByteReader::get_next_detailed_type() const
{
	if (ptr >= end) {
		[[unlikely]];
		return V2_ERROR;
	} else {
		return headerTranslation[*ptr];
	}
}

BITSCPP_INLINE bool ByteReader::is_next_integer() const
{
	return get_next_detailed_type() == V2_INT;
}
BITSCPP_INLINE bool ByteReader::is_next_floating_point() const
{
	return get_next_type() == V2_FLOAT;
}
BITSCPP_INLINE bool ByteReader::is_next_float16() const
{
	return get_next_detailed_type() == V2_DETAIL_HALF;
}
BITSCPP_INLINE bool ByteReader::is_next_bfloat16() const
{
	return get_next_detailed_type() == V2_DETAIL_BFLOAT;
}
BITSCPP_INLINE bool ByteReader::is_next_float32() const
{
	return get_next_detailed_type() == V2_FLOAT;
}
BITSCPP_INLINE bool ByteReader::is_next_float64() const
{
	return get_next_detailed_type() == V2_DETAIL_DOUBLE;
}
BITSCPP_INLINE bool ByteReader::is_next_bool() const
{
	return get_next_detailed_type() == V2_BOOLEAN;
}
BITSCPP_INLINE bool ByteReader::is_next_string() const
{
	return get_next_detailed_type() == V2_STRING;
}
BITSCPP_INLINE bool ByteReader::is_next_map() const
{
	return get_next_detailed_type() == V2_MAP;
}
BITSCPP_INLINE bool ByteReader::is_next_array() const
{
	return get_next_detailed_type() == V2_ARRAY;
}
BITSCPP_INLINE bool ByteReader::is_next_beg_object() const
{
	return get_next_detailed_type() == V2_OBJECT_BEGIN;
}
BITSCPP_INLINE bool ByteReader::is_next_end_object() const
{
	return get_next_detailed_type() == V2_OBJECT_END;
}

// strings
BITSCPP_INLINE ByteReader &
ByteReader::op_sized_byte_array_header(uint32_t &bytes)
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
		if (errors != 0) {
			[[unlikely]];
			bytes = 0;
			return *this;
		}
		bytes = size + IMMEDIATE_STRING_MAX_SIZE + 1;
		if (size > MAX_ARRAY_ELEMENTS - (IMMEDIATE_STRING_MAX_SIZE + 1)) {
			[[unlikely]];
			set_error(ERROR_ARRAY_TOO_BIG);
			bytes = 0;
			return *this;
		}
	} else {
		[[unlikely]];
		errors |= ERROR_TYPE_MISMATCH;
		bytes = 0;
	}

	return *this;
}
BITSCPP_INLINE ByteReader &ByteReader::op_sized_string_header(uint32_t &bytes)
{
	return op_sized_byte_array_header(bytes);
}

BITSCPP_INLINE ByteReader &ByteReader::op(std::string &str)
{
	std::string_view sv;
	op(sv);
	str = sv;
	return *this;
}
BITSCPP_INLINE ByteReader &ByteReader::op(std::string_view &str)
{
	uint32_t size = 0;
	op_sized_string_header(size);
	if (errors != 0) {
		[[unlikely]];
		str = {};
		return *this;
	}
	if (has_bytes_to_read(size)) {
		str = std::string_view((const char *)ptr, size);
		ptr += size;
	} else {
		[[unlikely]];
		errors |= ERROR_BUFFER_TOO_SMALL;
	}
	return *this;
}
BITSCPP_INLINE ByteReader &ByteReader::op_byte_array(char const **str,
													 uint32_t &size)
{
	assert(str);
	return op(*str, size);
}
BITSCPP_INLINE ByteReader &ByteReader::op(char const *&str, uint32_t &size)
{
	std::string_view sv;
	op(sv);
	str = sv.data();
	size = sv.size();
	return *this;
}

// byte array
BITSCPP_INLINE ByteReader &ByteReader::op_byte_array(uint8_t const **data,
													 uint32_t &bytes)
{
	assert(data);
	return op_byte_array((char const **)data, bytes);
}
BITSCPP_INLINE ByteReader &ByteReader::op_byte_array(uint8_t const *&data,
													 uint32_t &bytes)
{
	uint8_t const **data_p = &data;
	return op_byte_array(data_p, bytes);
}
BITSCPP_INLINE ByteReader &ByteReader::op_byte_array(std::vector<uint8_t> &data)
{
	data.clear();
	std::string_view sv;
	op(sv);
	if (errors == 0) {
		[[unlikely]];
		data.insert(data.end(), sv.begin(), sv.end());
	}
	return *this;
}
BITSCPP_INLINE ByteReader &ByteReader::op(std::vector<uint8_t> &data)
{
	return op_byte_array(data);
}
BITSCPP_INLINE ByteReader &ByteReader::op_byte_array(std::vector<char> &data)
{
	data.clear();
	std::string_view sv;
	op(sv);
	if (errors == 0) {
		[[unlikely]];
		data.insert(data.end(), sv.begin(), sv.end());
	}
	return *this;
}
BITSCPP_INLINE ByteReader &ByteReader::op(std::vector<char> &data)
{
	return op_byte_array(data);
}

// miscelanous
BITSCPP_INLINE ByteReader &ByteReader::op(bool &v) { return op_boolean(v); }
BITSCPP_INLINE ByteReader &ByteReader::op_boolean(bool &v)
{
	if (has_bytes_to_read(1) == false) {
		[[unlikely]];
		errors |= ERROR_BUFFER_TOO_SMALL;
		return *this;
	}
	const uint8_t header = *ptr;
	++ptr;
	if (header == BOOLEAN_TRUE) {
		v = true;
		return *this;
	} else if (header == BOOLEAN_FALSE) {
		v = false;
		return *this;
	}
	v = false;
	errors |= ERROR_TYPE_MISMATCH;
	return *this;
}
BITSCPP_INLINE ByteReader &ByteReader::op_begin_object()
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
BITSCPP_INLINE ByteReader &ByteReader::op_end_object()
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
BITSCPP_INLINE ByteReader &ByteReader::op(uint8_t &v)
{
	uint64_t vv = 0;
	op_uint(vv);
	v = vv;
	if (v != vv) {
		set_error(ERROR_INTEGER_OVERFLOW);
	}
	return *this;
}
BITSCPP_INLINE ByteReader &ByteReader::op(uint16_t &v)
{
	uint64_t vv = 0;
	op_uint(vv);
	v = vv;
	if (v != vv) {
		set_error(ERROR_INTEGER_OVERFLOW);
	}
	return *this;
}
BITSCPP_INLINE ByteReader &ByteReader::op(uint32_t &v)
{
	uint64_t vv = 0;
	op_uint(vv);
	v = vv;
	if (v != vv) {
		set_error(ERROR_INTEGER_OVERFLOW);
	}
	return *this;
}
BITSCPP_INLINE ByteReader &ByteReader::op(uint64_t &v)
{
	uint64_t vv = 0;
	op_uint(vv);
	v = vv;
	if (v != vv) {
		set_error(ERROR_INTEGER_OVERFLOW);
	}
	return *this;
}
BITSCPP_INLINE ByteReader &ByteReader::op(int8_t &v)
{
	int64_t vv = 0;
	op_int(vv);
	v = vv;
	if (v != vv) {
		set_error(ERROR_INTEGER_OVERFLOW);
	}
	return *this;
}
BITSCPP_INLINE ByteReader &ByteReader::op(int16_t &v)
{
	int64_t vv = 0;
	op_int(vv);
	v = vv;
	if (v != vv) {
		set_error(ERROR_INTEGER_OVERFLOW);
	}
	return *this;
}
BITSCPP_INLINE ByteReader &ByteReader::op(int32_t &v)
{
	int64_t vv = 0;
	op_int(vv);
	v = vv;
	if (v != vv) {
		set_error(ERROR_INTEGER_OVERFLOW);
	}
	return *this;
}
BITSCPP_INLINE ByteReader &ByteReader::op(int64_t &v)
{
	int64_t vv = 0;
	op_int(vv);
	v = vv;
	if (v != vv) {
		set_error(ERROR_INTEGER_OVERFLOW);
	}
	return *this;
}
BITSCPP_INLINE ByteReader &ByteReader::op(char &v)
{
	int64_t vv = 0;
	op_int(vv);
	v = vv;
	if (v != vv) {
		set_error(ERROR_INTEGER_OVERFLOW);
	}
	return *this;
}

BITSCPP_INLINE ByteReader &ByteReader::op_uint(uint64_t &v)
{
	int64_t vv = 0;
	op_int(vv);
	v = vv;
	return *this;
}
BITSCPP_INLINE ByteReader &ByteReader::op_int(int64_t &v)
{
	if (has_bytes_to_read(1) == false) {
		[[unlikely]];
		errors |= ERROR_BUFFER_TOO_SMALL;
		return *this;
	}

	const uint8_t header = *ptr;
	++ptr;
	uint64_t vv = 0;
	if (header <= END_IMMEDIATE_INTEGER) {
		v = (int64_t)(uint32_t)header + (int64_t)IMMEDIATE_INTEGER_VALUE_MIN;
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
		const uint8_t h = ((uint64_t)(header - BEG_12B_INTEGER));
		assert(h < 16);
		vv |= h;
	} else if (header <= END_SIZED_INTEGER) {
		const int bytes = header - BEG_SIZED_INTEGER + 2;
		assert(bytes >= 2);
		assert(bytes <= 8);
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

	const uint64_t sign = ((int64_t)(vv << 63)) >> 63;
	assert((vv & 1) ? sign == -1ll : sign == 0);
	const uint64_t abs = vv >> 1;
	v = sign ^ abs;
	static_assert(-1ll == (-10000000ll >> 63));

	return *this;
}

// floats
BITSCPP_INLINE ByteReader &ByteReader::op_half(float &v)
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
BITSCPP_INLINE ByteReader &ByteReader::op_bfloat(float &v)
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
BITSCPP_INLINE ByteReader &ByteReader::op_float(float &v)
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
BITSCPP_INLINE ByteReader &ByteReader::op_double(double &v)
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
BITSCPP_INLINE ByteReader &ByteReader::op(float &v)
{
	if (has_bytes_to_read(1) == false) {
		[[unlikely]];
		errors |= ERROR_BUFFER_TOO_SMALL;
		return *this;
	}
	const uint8_t header = *ptr;
	++ptr;
	double d = v;
	switch (header) {
	case BEG_HALF:
		if (has_bytes_to_read(2) == false) {
			[[unlikely]];
			errors |= ERROR_BUFFER_TOO_SMALL;
			return *this;
		}
		_read_half(ptr, v);
		ptr += 2;
		break;
	case BEG_BFLOAT:
		if (has_bytes_to_read(2) == false) {
			[[unlikely]];
			errors |= ERROR_BUFFER_TOO_SMALL;
			return *this;
		}
		_read_bfloat(ptr, v);
		ptr += 2;
		break;
	case BEG_FLOAT:
		if (has_bytes_to_read(4) == false) {
			[[unlikely]];
			errors |= ERROR_BUFFER_TOO_SMALL;
			return *this;
		}
		_read_float(ptr, v);
		ptr += 4;
		break;
	case BEG_DOUBLE:
		if (has_bytes_to_read(8) == false) {
			[[unlikely]];
			errors |= ERROR_BUFFER_TOO_SMALL;
			return *this;
		}
		_read_double(ptr, d);
		v = d;
		ptr += 8;
		break;
	default:
		[[unlikely]];
		errors |= ERROR_TYPE_MISMATCH;
		return *this;
	}
	return *this;
}
BITSCPP_INLINE ByteReader &ByteReader::op(double &v)
{
	if (has_bytes_to_read(1) == false) {
		[[unlikely]];
		errors |= ERROR_BUFFER_TOO_SMALL;
		return *this;
	}
	const uint8_t header = *ptr;
	++ptr;
	float f = v;
	switch (header) {
	case BEG_HALF:
		if (has_bytes_to_read(2) == false) {
			[[unlikely]];
			errors |= ERROR_BUFFER_TOO_SMALL;
			return *this;
		}
		_read_half(ptr, f);
		v = f;
		ptr += 2;
		break;
	case BEG_BFLOAT:
		if (has_bytes_to_read(2) == false) {
			[[unlikely]];
			errors |= ERROR_BUFFER_TOO_SMALL;
			return *this;
		}
		_read_bfloat(ptr, f);
		v = f;
		ptr += 2;
		break;
	case BEG_FLOAT:
		if (has_bytes_to_read(4) == false) {
			[[unlikely]];
			errors |= ERROR_BUFFER_TOO_SMALL;
			return *this;
		}
		_read_float(ptr, f);
		v = f;
		ptr += 4;
		break;
	case BEG_DOUBLE:
		if (has_bytes_to_read(8) == false) {
			[[unlikely]];
			errors |= ERROR_BUFFER_TOO_SMALL;
			return *this;
		}
		_read_double(ptr, v);
		ptr += 8;
		break;
	default:
		[[unlikely]];
		errors |= ERROR_TYPE_MISMATCH;
		return *this;
	}
	return *this;
}
BITSCPP_INLINE void ByteReader::_read_half(const uint8_t *ptr, float &v)
{
	v = Float16ToFloat32(ReadBytesInNetworkOrder(ptr, 2));
}
BITSCPP_INLINE void ByteReader::_read_bfloat(const uint8_t *ptr, float &v)
{
	const uint16_t bfloat = ReadBytesInNetworkOrder(ptr, 2);
	uint32_t ffloat = ((uint32_t)bfloat) << 16;
	v = std::bit_cast<float>(ffloat);
}
BITSCPP_INLINE void ByteReader::_read_float(const uint8_t *ptr, float &v)
{
	const uint32_t vv = ReadBytesInNetworkOrder(ptr, 4);
	v = std::bit_cast<float>(vv);
}
BITSCPP_INLINE void ByteReader::_read_double(const uint8_t *ptr, double &v)
{
	const uint64_t vv = ReadBytesInNetworkOrder(ptr, 8);
	v = std::bit_cast<double>(vv);
}

// map
BITSCPP_INLINE ByteReader &ByteReader::op_map_header(uint32_t &elements)
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
		if (errors != 0) {
			[[unlikely]];
			elements = 0;
			return *this;
		}
		if (size > MAX_ARRAY_ELEMENTS - 1) {
			[[unlikely]];
			set_error(ERROR_ARRAY_TOO_BIG);
			elements = 0;
			return *this;
		}
		elements = size + 1;
		if (has_bytes_to_read(elements) == false) {
			set_error(ERROR_ARRAY_TOO_BIG);
			set_error(ERROR_TYPE_MISMATCH);
			set_error(ERROR_BUFFER_TOO_SMALL);
		}
	} else if (header == BEG_MAP_EMPTY) {
		elements = 0;
	} else {
		[[unlikely]];
		elements = 0;
		errors |= ERROR_TYPE_MISMATCH;
	}
	return *this;
}

BITSCPP_INLINE ByteReader &ByteReader::op_array_header(uint32_t &elements)
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
		if (errors != 0) {
			[[unlikely]];
			elements = 0;
			return *this;
		}
		elements = size + IMMEDIATE_ARRAY_MAX_SIZE + 1;
		if (size > MAX_ARRAY_ELEMENTS - (IMMEDIATE_ARRAY_MAX_SIZE + 1)) {
			[[unlikely]];
			set_error(ERROR_ARRAY_TOO_BIG);
			elements = 0;
			return *this;
		}
		assert(elements == size + IMMEDIATE_ARRAY_MAX_SIZE + 1);
		if (has_bytes_to_read(elements) == false) {
			set_error(ERROR_ARRAY_TOO_BIG);
			set_error(ERROR_TYPE_MISMATCH);
			set_error(ERROR_BUFFER_TOO_SMALL);
		}
	} else {
		[[unlikely]];
		errors |= ERROR_TYPE_MISMATCH;
		elements = 0;
	}
	return *this;
}

BITSCPP_INLINE ByteReader &ByteReader::op_untyped_var_uint(uint64_t &v)
{
	if (has_bytes_to_read(1) == false) {
		[[unlikely]];
		errors |= ERROR_BUFFER_TOO_SMALL;
		return *this;
	}
	const uint8_t header = *ptr;
	++ptr;
	const int32_t bytes = std::countl_one(header);
	if (bytes >= 1) {
		if (has_bytes_to_read(bytes) == false) {
			[[unlikely]];
			errors |= ERROR_BUFFER_TOO_SMALL;
			return *this;
		}
		constexpr uint8_t masks[] = {0x7F, 0x3F, 0x1F, 0x0F, 0x07,
									 0x03, 0x01, 0x00, 0x00};
		constexpr uint8_t shifts[] = {7, 6, 5, 4, 3, 2, 1, 0, 0};
		v = ReadBytesInNetworkOrder(ptr, bytes) << shifts[bytes];
		ptr += bytes;
		v |= header & masks[bytes];
	} else {
		v = header;
	}
	return *this;
}
BITSCPP_INLINE ByteReader &ByteReader::op_untyped_var_int(int64_t &v)
{
	uint64_t vv = 0;
	op_untyped_var_uint(vv);
	const uint64_t sign = vv & 1;
	const uint64_t abs = vv >> 1;
	vv = sign ? ~abs : abs;
	v = vv;
	return *this;
}
BITSCPP_INLINE ByteReader &ByteReader::op_untyped_uint32(uint32_t &v)
{
	if (has_bytes_to_read(4)) {
		[[likely]];
		v = ReadBytesInNetworkOrder(ptr, 4);
		ptr += 4;
	} else {
		errors |= ERROR_BUFFER_TOO_SMALL;
	}
	return *this;
}

BITSCPP_INLINE ByteReader &ByteReader::skip(uint32_t bytes)
{
	if (has_bytes_to_read(bytes) == false) {
		[[unlikely]];
		errors |= ERROR_BUFFER_TOO_SMALL;
		return *this;
	}
	ptr += bytes;
	return *this;
}
BITSCPP_INLINE bool ByteReader::is_valid() const { return errors == ERROR_OK; }
BITSCPP_INLINE Errors ByteReader::get_errors() const { return (Errors)errors; }
BITSCPP_INLINE bool ByteReader::has_any_more() const { return ptr != end; }
BITSCPP_INLINE uint32_t ByteReader::get_offset() const { return ptr - _buffer; }
BITSCPP_INLINE const uint8_t *ByteReader::get_buffer() const { return _buffer; }
BITSCPP_INLINE uint32_t ByteReader::get_remaining_bytes() const
{
	return end - ptr;
}
BITSCPP_INLINE bool ByteReader::has_bytes_to_read(uint32_t bytes) const
{
	return bytes <= end - ptr;
}

BITSCPP_INLINE void ByteReader::set_error(Errors error) { errors |= error; }

} // namespace v2
} // namespace bitscpp

#endif
