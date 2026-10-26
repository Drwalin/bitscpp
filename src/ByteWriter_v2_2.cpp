// Copyright (C) 2023-2026 Marek Zalewski aka Drwalin
//
// This file is part of bitscpp project under MIT License
// You should have received a copy of the MIT License along with this program.

#pragma once

#ifndef BITSCPP_BYTE_WRITER_V2_2_CPP
#define BITSCPP_BYTE_WRITER_V2_2_CPP

#if not defined _BITSCPP_INLINE
#define BITSCPP_NO_INLINE_CPP_BYTE_WRITER_V2_2
#endif

#include <cassert>
#include <cstring>

#include "../thirdparty/half_float/HalfFloat.hpp"

#include "../include/bitscpp/Endianness.hpp"
#include "../include/bitscpp/V2_Specification.hpp"

#include "../include/bitscpp/ByteWriter_v2_2.hpp"

# undef BITSCPP_INLINE
#if defined BITSCPP_NO_INLINE_CPP_BYTE_WRITER_V2_2
# define BITSCPP_INLINE
#else
# define BITSCPP_INLINE _BITSCPP_INLINE
#endif

// BITSCPP_INLINE inline void asdf() {}

namespace bitscpp
{
namespace v2_2
{
using namespace bitscpp::v2;

BITSCPP_INLINE void ByteWriter::Init(GenericBufferWriter &&buf)
{
	_buffer = std::move(buf);
}

BITSCPP_INLINE ByteWriter::ByteWriter(GenericBufferWriterPimpl impl)
	: _buffer(impl)
{
	Init({impl});
}

BITSCPP_INLINE ByteWriter::ByteWriter(GenericBufferWriter &&buf)
	: _buffer(std::move(buf))
{
}

BITSCPP_INLINE ByteWriter::ByteWriter(ByteWriter &&o)
	: _buffer(std::move(o._buffer)), errors(o.errors)
{
	o._buffer = {{}};
	o.errors = 0;
}

BITSCPP_INLINE ByteWriter &ByteWriter::operator=(ByteWriter &&o)
{
	_buffer = std::move(o._buffer);
	errors = o.errors;
	o._buffer = {{}};
	o.errors = 0;
	return *this;
}

BITSCPP_INLINE void *ByteWriter::release_buffer_impl()
{
	return _buffer.release_buffer();
}

BITSCPP_INLINE ByteWriter &
ByteWriter::op_sized_byte_array_header(uint32_t bytes)
{
	if (bytes <= IMMEDIATE_STRING_MAX_SIZE) {
		_append_byte(BEG_STRING_IMMEDIATE_SIZED + (uint8_t)bytes);
	} else {
		_append_byte(BEG_STRING_VAR_SIZED);
		op_untyped_var_uint(bytes - IMMEDIATE_STRING_MAX_SIZE - 1);
	}
	return *this;
}

BITSCPP_INLINE ByteWriter &ByteWriter::op_sized_string_header(uint32_t bytes)
{
	return op_sized_byte_array_header(bytes);
}

BITSCPP_INLINE ByteWriter &ByteWriter::op(const std::string &str)
{
	return op(str.c_str(), str.size());
}

BITSCPP_INLINE ByteWriter &ByteWriter::op(const std::string_view str)
{
	return op_byte_array((const uint8_t *)str.data(), str.size());
}

BITSCPP_INLINE ByteWriter &ByteWriter::op(const char *str)
{
	return op_byte_array((const uint8_t *)str, strlen(str));
}

BITSCPP_INLINE ByteWriter &ByteWriter::op(const char *str, uint32_t size)
{
	return op_byte_array((const uint8_t *)str, size);
}

BITSCPP_INLINE ByteWriter &ByteWriter::op(char *str)
{
	return op_byte_array((const uint8_t *)str, strlen(str));
}

BITSCPP_INLINE ByteWriter &ByteWriter::op(char *str, uint32_t size)
{
	return op_byte_array((const uint8_t *)str, size);
}

BITSCPP_INLINE ByteWriter &ByteWriter::op_byte_array(const uint8_t *data,
													 uint32_t bytes)
{
	_reserve_expand(bytes + 6);
	op_sized_byte_array_header(bytes);
	_append(data, bytes);
	return *this;
}

BITSCPP_INLINE ByteWriter &
ByteWriter::op_byte_array(const std::vector<uint8_t> &data)
{
	return op_byte_array(data.data(), data.size());
}

BITSCPP_INLINE ByteWriter &ByteWriter::op(const std::vector<uint8_t> &data)
{
	return op_byte_array(data);
}

BITSCPP_INLINE ByteWriter &
ByteWriter::op_byte_array(const std::vector<char> &data)
{
	return op_byte_array((const uint8_t *)data.data(), data.size());
}

BITSCPP_INLINE ByteWriter &ByteWriter::op(const std::vector<char> &data)
{
	return op_byte_array(data);
}

BITSCPP_INLINE ByteWriter &ByteWriter::op(bool v) { return op_boolean(v); }

BITSCPP_INLINE ByteWriter &ByteWriter::op_boolean(bool v)
{
	_append_byte(v ? BOOLEAN_TRUE : BOOLEAN_FALSE);
	return *this;
}

BITSCPP_INLINE ByteWriter &ByteWriter::op_false() { return op_boolean(false); }

BITSCPP_INLINE ByteWriter &ByteWriter::op_true() { return op_boolean(true); }

BITSCPP_INLINE ByteWriter &ByteWriter::op_begin_object()
{
	_append_byte(BEG_OBJECT);
	return *this;
}

BITSCPP_INLINE ByteWriter &ByteWriter::op_end_object()
{
	_append_byte(END_OBJECT);
	return *this;
}

BITSCPP_INLINE ByteWriter &ByteWriter::op(uint8_t v) { return op_uint(v); }

BITSCPP_INLINE ByteWriter &ByteWriter::op(uint16_t v) { return op_uint(v); }

BITSCPP_INLINE ByteWriter &ByteWriter::op(uint32_t v) { return op_uint(v); }

BITSCPP_INLINE ByteWriter &ByteWriter::op(uint64_t v) { return op_uint(v); }

BITSCPP_INLINE ByteWriter &ByteWriter::op(int8_t v) { return op_int(v); }

BITSCPP_INLINE ByteWriter &ByteWriter::op(int16_t v) { return op_int(v); }

BITSCPP_INLINE ByteWriter &ByteWriter::op(int32_t v) { return op_int(v); }

BITSCPP_INLINE ByteWriter &ByteWriter::op(int64_t v) { return op_int(v); }

BITSCPP_INLINE ByteWriter &ByteWriter::op(char v) { return op_int(v); }

BITSCPP_INLINE ByteWriter &ByteWriter::op_uint(uint64_t v) { return op_int(v); }

BITSCPP_INLINE ByteWriter &ByteWriter::op_int(int64_t v)
{
	if (v >= IMMEDIATE_INTEGER_VALUE_MIN && v <= IMMEDIATE_INTEGER_VALUE_MAX)
		[[likely]] {
		v -= IMMEDIATE_INTEGER_VALUE_MIN;
		assert(v >= 0 && v <= IMMEDIATE_INTEGER_MAX);
		_append_byte((uint8_t)(uint64_t)v);
		return *this;
	}

	uint64_t uv = (uint64_t)v;
	uv = (uv << 1) ^ (v >> 63);
	assert(uv == (v < 0 ? (((~v) << 1) | 1) : v << 1));
	static_assert(-1ll == (-10000000ll >> 63));
	const uint32_t bits = std::bit_width(uv);
	if (bits <= 12) [[likely]] {
		const uint64_t low = uv & 0xF;
		const uint64_t high = uv >> 4;
		assert(low < 16);
		assert(high < 256);
		uint8_t *p = _expand(2);
		p[0] = BEG_12B_INTEGER + (uint8_t)low;
		p[1] = (uint8_t)high;
	} else {
		const int bytes = (bits + 7) >> 3;
		uint8_t *p = _expand(bytes + 1);
		assert(bytes >= 2);
		assert(bytes <= 8);
		*p = bytes + BEG_SIZED_INTEGER - 2;
		assert(*p >= BEG_SIZED_INTEGER && *p <= END_SIZED_INTEGER);
		WriteBytesInNetworkOrder(p + 1, uv, bytes);
	}

	return *this;
}

// floats

BITSCPP_INLINE ByteWriter &ByteWriter::op_half(float value)
{
	uint8_t *p = _expand(3);
	*p = BEG_HALF;
	WriteBytesInNetworkOrder(p + 1, Float32ToFloat16(value), 2);
	return *this;
}

BITSCPP_INLINE ByteWriter &ByteWriter::op_bfloat(float value)
{
	uint8_t *p = _expand(3);

	constexpr uint32_t exponentMask = 0x7F800000;
	constexpr uint32_t fractionMask = 0x007FFFFF;
	const uint32_t bv32 = std::bit_cast<uint32_t>(value);

	const uint32_t exponent = bv32 & exponentMask;
	const uint32_t fraction = bv32 & fractionMask;

	p[0] = BEG_BFLOAT;
	if (exponent == exponentMask) [[unlikely]] {
		if (fraction == 0) { // infinity
			p[1] = 0x80;
		} else { // not a number
			p[1] = 0xFF;
		}
		p[2] = bv32 >> 24;
		return *this;
	} else [[likely]] {
		const uint16_t bfloat = bv32 >> 16;
		WriteBytesInNetworkOrder(p + 1, bfloat, 2);
		return *this;
	}
}

BITSCPP_INLINE ByteWriter &ByteWriter::op(float value)
{
	return op_float(value);
}

BITSCPP_INLINE ByteWriter &ByteWriter::op(double value)
{
	return op_double(value);
}

BITSCPP_INLINE ByteWriter &ByteWriter::op_float(float value)
{
	const uint32_t bv32 = std::bit_cast<uint32_t>(value);
	uint8_t *p = _expand(5);
	p[0] = BEG_FLOAT;
	WriteBytesInNetworkOrder(p + 1, bv32, 4);
	return *this;
}

BITSCPP_INLINE ByteWriter &ByteWriter::op_double(double value)
{
	const uint64_t bv64 = std::bit_cast<uint64_t>(value);
	uint8_t *p = _expand(9);
	p[0] = BEG_DOUBLE;
	WriteBytesInNetworkOrder(p + 1, bv64, 8);
	return *this;
}

BITSCPP_INLINE ByteWriter &ByteWriter::op_map_header(uint32_t elements)
{
	if (elements == 0) {
		_append_byte(BEG_MAP_EMPTY);
	} else {
		_reserve_expand(10);
		_append_byte(BEG_MAP_SIZED);
		op_untyped_var_uint(elements - 1);
	}
	return *this;
}

BITSCPP_INLINE ByteWriter &ByteWriter::op_array_header(uint32_t elements)
{
	_reserve_expand(10);
	if (elements <= IMMEDIATE_ARRAY_MAX_SIZE)
		[[likely]] { // size embeded in header
		_append_byte(BEG_ARRAY_IMMEDIATE_SIZED + elements);
	} else { // size in following VAR_INT+17
		_append_byte(BEG_ARRAY_VAR_SIZED);
		op_untyped_var_uint(elements - IMMEDIATE_ARRAY_MAX_SIZE - 1);
	}
	return *this;
}

BITSCPP_INLINE ByteWriter &ByteWriter::op_untyped_var_uint(uint64_t value)
{
	if (value <= 0x7F) [[likely]] {
		uint8_t byte = (uint8_t)value;
		assert(byte == value);
		_append_byte(byte);
	} else {
		const uint32_t bits = std::bit_width(value);
		constexpr uint8_t _bytes[65] = {
			1,1,1,1,1,1,1,
			2,2,2,2,2,2,2,
			3,3,3,3,3,3,3,
			4,4,4,4,4,4,4,
			5,5,5,5,5,5,5,
			6,6,6,6,6,6,6,
			7,7,7,7,7,7,7,
			8,8,8,8,8,8,8,8,
			9,9,9,9,9,9,9,9};
		const uint32_t bytes = _bytes[bits];
		assert(bytes == (bits + 6) / 7);

		constexpr uint8_t masks[9] = {0x7F, 0x3F, 0x1F, 0x0F,
									 0x07, 0x03, 0x01, 0x00, 0x00};
		constexpr uint8_t ones[9] = {0x00, 0x80, 0xC0, 0xE0,
									0xF0, 0xF8, 0xFC, 0xFE, 0xFF};
		constexpr uint8_t shifts[9] = {7, 6, 5, 4, 3, 2, 1, 0, 0};

		uint8_t *p = _expand(bytes);

		const uint8_t header = (masks[bytes-1] & value) | ones[bytes-1];
		p[0] = header;

		value >>= shifts[bytes-1];
		WriteBytesInNetworkOrder(p + 1, value, bytes - 1);
	}
	return *this;
}

BITSCPP_INLINE ByteWriter &ByteWriter::op_untyped_var_int(int64_t value)
{
	const uint64_t uvalue = value;
	const uint64_t sign = uvalue >> 63;
	const uint64_t abs = sign ? ~uvalue : uvalue;
	return op_untyped_var_uint((abs << 1) | sign);
}

BITSCPP_INLINE ByteWriter &ByteWriter::op_untyped_uint32(uint32_t v)
{
	uint8_t *ptr = _expand(4);
	WriteBytesInNetworkOrder(ptr, v, 4);
	return *this;
}

BITSCPP_INLINE void ByteWriter::_append_byte(const uint8_t byte)
{
	_buffer.append_byte(byte);
}

BITSCPP_INLINE void ByteWriter::_append(const uint8_t *data, uint32_t bytes)
{
	_buffer.append(data, bytes);
}

BITSCPP_INLINE uint8_t *ByteWriter::_expand(uint8_t bytesToExpand)
{
	return _buffer.expand(bytesToExpand);
}

BITSCPP_INLINE void ByteWriter::_reserve_expand(size_t bytesToExpand)
{
	return _buffer.expand_hint(bytesToExpand);
}

BITSCPP_INLINE Errors ByteWriter::get_errors() const { return (Errors)errors; }

BITSCPP_INLINE void ByteWriter::set_error(Errors error) { errors |= error; }

} // namespace v2_2
} // namespace bitscpp

#endif
