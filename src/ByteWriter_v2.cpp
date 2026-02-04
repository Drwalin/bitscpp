// Copyright (C) 2023-2026 Marek Zalewski aka Drwalin
//
// This file is part of bitscpp project under MIT License
// You should have received a copy of the MIT License along with this program.

#include <cassert>

#include "../thirdpart/half_float/HalfFloat.hpp"

#include "../include/bitscpp/Endianness.hpp"
#include "../include/bitscpp/V2_Specification.hpp"

#include "../include/bitscpp/ByteWriter_v2.hpp"

#define ByteWriter BITSCPP_CONCATENATE_NAMES(ByteWriter, BITSCPP_BYTE_WRITER_V2_NAME_SUFFIX)

namespace bitscpp
{
namespace v2
{
ByteWriter &ByteWriter::op_sized_byte_array_header(uint32_t bytes)
{
	if (bytes <= IMMEDIATE_STRING_MAX_SIZE) {
		_append_byte(BEG_STRING_IMMEDIATE_SIZED + (uint8_t)bytes);
	} else {
		_append_byte(BEG_STRING_VAR_SIZED);
		op_untyped_var_uint(bytes - IMMEDIATE_STRING_MAX_SIZE - 1);
	}
	return *this;
}
ByteWriter &ByteWriter::op_sized_string_header(uint32_t bytes)
{
	return op_sized_byte_array_header(bytes);
}
ByteWriter &ByteWriter::op_cstring_header()
{
	_append_byte(V2_CSTRING);
	return *this;
}

ByteWriter &ByteWriter::op_sized_string(const std::string &str)
{
	return op_sized_string(str.c_str(), str.size());
}
ByteWriter &ByteWriter::op_sized_string(const std::string_view str)
{
	return op_sized_string(str.data(), str.size());
}
ByteWriter &ByteWriter::op_sized_string(const char *str)
{
	return op_sized_string(str, strlen(str));
}
ByteWriter &ByteWriter::op_sized_string(const char *str, uint32_t size)
{
	return op_byte_array((const uint8_t *)str, size);
}
ByteWriter &ByteWriter::op_cstring(const char *str)
{
	return op_cstring(str, strlen(str));
}
ByteWriter &ByteWriter::op_cstring(const char *str, uint32_t size)
{
	_reserve_expand(size + 2);
	op_cstring_header();
	_append((const uint8_t *)str, size + 1);
	return *this;
}
ByteWriter &ByteWriter::op(const std::string &str)
{
	return op_sized_string(str);
}
ByteWriter &ByteWriter::op(const std::string_view str)
{
	return op_sized_string(str);
}
ByteWriter &ByteWriter::op(const char *str)
{
	return op_cstring(str);
}

ByteWriter &ByteWriter::op_byte_array(const uint8_t *data, uint32_t bytes)
{
	_reserve_expand(bytes + 6);
	op_sized_byte_array_header(bytes);
	_append(data, bytes);
	return *this;
}
ByteWriter &ByteWriter::op(const std::vector<uint8_t> &data)
{
	return op_byte_array(data.data(), data.size());
}

ByteWriter &ByteWriter::op(bool v) { return op_boolean(v); }
ByteWriter &ByteWriter::op_boolean(bool v)
{
	_append_byte(v ? BOOLEAN_TRUE : BOOLEAN_FALSE);
	return *this;
}
ByteWriter &ByteWriter::op_false() { return op_boolean(false); }
ByteWriter &ByteWriter::op_true() { return op_boolean(true); }
ByteWriter &ByteWriter::op_begin_object()
{
	_append_byte(BEG_OBJECT);
	return *this;
}
ByteWriter &ByteWriter::op_end_object()
{
	_append_byte(END_OBJECT);
	return *this;
}

ByteWriter &ByteWriter::op(uint8_t v) { return op_uint(v); }
ByteWriter &ByteWriter::op(uint16_t v) { return op_uint(v); }
ByteWriter &ByteWriter::op(uint32_t v) { return op_uint(v); }
ByteWriter &ByteWriter::op(uint64_t v) { return op_uint(v); }
ByteWriter &ByteWriter::op(int8_t v) { return op_int(v); }
ByteWriter &ByteWriter::op(int16_t v) { return op_int(v); }
ByteWriter &ByteWriter::op(int32_t v) { return op_int(v); }
ByteWriter &ByteWriter::op(int64_t v) { return op_int(v); }
ByteWriter &ByteWriter::op(char v) { return op_int(v); }

ByteWriter &ByteWriter::op_uint(uint64_t v) { return op_int(v); }
ByteWriter &ByteWriter::op_int(int64_t v)
{
	if (v >= IMMEDIATE_INTEGER_VALUE_MIN && v <= IMMEDIATE_INTEGER_VALUE_MAX) {
		v -= IMMEDIATE_INTEGER_VALUE_MIN;
		assert(v >= 0 && v <= IMMEDIATE_INTEGER_MAX);
		_append_byte((uint8_t)(uint64_t)v);
	} else {
		uint64_t uv = (uint64_t)v;
		uv = v < 0 ? ((~uv) << 1) | 1 : uv << 1;
		const int bits = std::bit_width(uv);
		if (bits <= 12) {
			const uint64_t low = uv & 0xF;
			const uint64_t high = uv >> 4;
			assert(low < 16);
			assert(high < 256);
			const uint32_t offset = _expand(2);
			ptr[offset] = BEG_12B_INTEGER + (uint8_t)low;
			ptr[offset + 1] = (uint8_t)high;
		} else {
			const int bytes = (bits + 7) >> 3;
			uint32_t offset = _expand(bytes + 1);
			assert(bytes >= 2);
			assert(bytes <= 8);
			ptr[offset] = BEG_SIZED_INTEGER + bytes - 2;
			assert(ptr[offset] >= BEG_SIZED_INTEGER &&
				   ptr[offset] <= END_SIZED_INTEGER);
			offset++;
			
			WriteBytesInNetworkOrder(ptr+offset, uv, bytes);
			assert(ptr[offset-1] != 0);
		}
	}
	return *this;
}

// floats
ByteWriter &ByteWriter::op_half(float value)
{
	uint32_t offset = _expand(3);
	ptr[offset] = BEG_HALF;
	WriteBytesInNetworkOrder(ptr + offset + 1, Float32ToFloat16(value), 2);
	return *this;
}
ByteWriter &ByteWriter::op_bfloat(float value)
{
	const uint32_t offset = _expand(3);

	constexpr uint32_t exponentMask = 0x7F800000;
	constexpr uint32_t fractionMask = 0x007FFFFF;
	const uint32_t bv32 = std::bit_cast<uint32_t>(value);

	const uint32_t exponent = bv32 & exponentMask;
	const uint32_t fraction = bv32 & fractionMask;

	if (exponent == exponentMask) {
		[[unlikely]];
		ptr[offset + 1] = fraction == 0 ? 0x80 : 0xFF;
		ptr[offset] = BEG_BFLOAT;
		ptr[offset + 2] = 0x7F;
		return *this;
	} else {
		[[likely]];
		const uint16_t bfloat = bv32 >> 16;
		ptr[offset] = BEG_BFLOAT;
		WriteBytesInNetworkOrder(ptr + offset + 1, bfloat, 2);
		return *this;
	}
}
ByteWriter &ByteWriter::op(float value) { return op_float(value); }
ByteWriter &ByteWriter::op(double value) { return op_double(value); }
ByteWriter &ByteWriter::op_float(float value)
{
	const uint32_t bv32 = std::bit_cast<uint32_t>(value);
	uint32_t offset = _expand(5);
	ptr[offset] = BEG_FLOAT;
	WriteBytesInNetworkOrder(ptr + offset + 1, bv32, 4);
	return *this;
}
ByteWriter &ByteWriter::op_double(double value)
{
	const uint64_t bv64 = std::bit_cast<uint64_t>(value);
	uint32_t offset = _expand(9);
	ptr[offset] = BEG_DOUBLE;
	WriteBytesInNetworkOrder(ptr + offset + 1, bv64, 8);
	return *this;
}

ByteWriter &ByteWriter::op_map_header(uint32_t elements)
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

ByteWriter &ByteWriter::op_array_header(uint32_t elements)
{
	_reserve_expand(10);
	if (elements <= 17) { // size embeded in header
		_append_byte(BEG_ARRAY_IMMEDIATE_SIZED + elements);
	} else { // size in following VAR_INT+17
		_append_byte(BEG_ARRAY_VAR_SIZED);
		op_untyped_var_uint(elements - 18);
	}
	return *this;
}

ByteWriter &ByteWriter::op_untyped_var_uint(uint64_t value)
{
	if (value <= 0x7F) {
		uint8_t byte = (uint8_t)value;
		_append(&byte, 1);
	} else {
		const uint32_t bits = std::bit_width(value);
		const uint32_t bytes = (bits + 6) / 7;

		constexpr uint8_t masks[] = {0x00, 0x7F, 0x3F, 0x1F, 0x0F,
									 0x07, 0x03, 0x01, 0x00, 0x00};
		constexpr uint8_t ones[] = {0x00, 0x00, 0x80, 0xC0, 0xE0,
									0xF0, 0xF8, 0xFC, 0xFE, 0xFF};
		constexpr uint8_t shifts[] = {0, 7, 6, 5, 4, 3, 2, 1, 0, 0};

		const uint32_t offset = _expand(bytes);

		const uint8_t header = (masks[bytes] & value) | ones[bytes];
		ptr[offset] = header;

		value >>= shifts[bytes];
		WriteBytesInNetworkOrder(ptr + offset + 1, value, bytes - 1);
	}
	return *this;
}
ByteWriter &ByteWriter::op_untyped_var_int(int64_t value)
{
	const uint64_t uvalue = value;
	const uint64_t sign = uvalue >> 63;
	const uint64_t abs = sign ? ~uvalue : uvalue;
	return op_untyped_var_uint((abs << 1) | sign);
}

void ByteWriter::_append_byte(const uint8_t byte)
{
	uint32_t oldSize = _expand(1);
	ptr[oldSize] = byte;
}

void ByteWriter::_append(const uint8_t *data, uint32_t bytes)
{
	uint32_t oldSize = _expand(bytes);
	memcpy(ptr + oldSize, data, bytes);
}

size_t ByteWriter::_expand(size_t bytesToExpand)
{
	size_t oldSize = _buffer->size();
	_buffer->resize(oldSize + bytesToExpand);
	ptr = _buffer->data();
	return oldSize;
}

void ByteWriter::_reserve_expand(size_t bytesToExpand)
{
	_reserve(_buffer->size() + bytesToExpand);
}

void ByteWriter::_reserve(size_t newCapacity)
{
	_buffer->reserve((newCapacity * 3) / 2 + 16);
	ptr = _buffer->data();
}

} // namespace v2
} // namespace bitscpp

#undef ByteWriter
