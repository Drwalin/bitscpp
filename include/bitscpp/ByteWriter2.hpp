// Copyright (C) 2023-2026 Marek Zalewski aka Drwalin
//
// This file is part of bitscpp project under MIT License
// You should have received a copy of the MIT License along with this program.

#ifndef BITSCPP_BYTE_WRITER_V2_HPP
#define BITSCPP_BYTE_WRITER_V2_HPP

#include <bit>
#include <cstdint>
#include <cstring>

#include <string>
#include <string_view>
#include <vector>

#include "Endianness.hpp"

namespace bitscpp
{
namespace v2
{

template <typename BT> class ByteWriter;

template <typename BT, typename T>
inline ByteWriter<BT> &op(ByteWriter<BT> &writer, const T &data)
{
	(*(T *)&data).__ByteStream_op(writer);
	return writer;
}

namespace impl
{
template <typename BT, typename T>
static inline ByteWriter<BT> &__op_ptr(ByteWriter<BT> &writer, T *const data)
{
	op(writer, *data);
	return writer;
}

template <typename BT, typename T>
static inline ByteWriter<BT> &__op_ref(ByteWriter<BT> &writer, const T &data)
{
	op(writer, data);
	return writer;
}
} // namespace impl

/*
 * BT requires following interface:
 * class BT {
 *   uint8_t *data();
 *   size_t size();
 *   void resize(size_t newSize);
 *   size_t capacity();
 *   void reserve(size_t newCapacity);
 * };
 *
 * behavior should be similar to std::vector<uint8_t>
 */
template <typename BT = std::vector<uint8_t>> class ByteWriter
{
public:
	template <typename T> inline ByteWriter &op(T *const data)
	{
		impl::__op_ptr(*this, data);
		return *this;
	}

	template <typename T> inline ByteWriter &op(const T &data)
	{
		impl::__op_ref(*this, data);
		return *this;
	}

public:
	inline uint32_t GetSize() const { return _buffer->size(); }

	void Init(BT *buffer)
	{
		this->_buffer = buffer;
		ptr = _buffer->data();
	}

	inline ByteWriter() : _buffer(nullptr), ptr(nullptr) {}

	inline ByteWriter(BT &buffer) { Init(&buffer); }

	inline ByteWriter(BT *buffer) { Init(buffer); }

	// strings
	inline ByteWriter &op_sized_byte_array_header(const std::string &str);
	inline ByteWriter &op_sized_string_header(const std::string &str);
	inline ByteWriter &op_cstring_header();
	
	inline ByteWriter &op_sized_string(const std::string &str);
	inline ByteWriter &op_sized_string(const std::string_view str);
	inline ByteWriter &op_sized_string(const char *str);
	inline ByteWriter &op_sized_string(const char *str, uint32_t size);
	inline ByteWriter &op_cstring(const char *str);
	inline ByteWriter &op_cstring(const char *str, uint32_t size);
	
	// constant size byte array
	inline ByteWriter &op_byte_array(const uint8_t *data, uint32_t bytes);
	inline ByteWriter &op_byte_array(const int8_t *data, uint32_t bytes);

	inline ByteWriter &op(bool v);
	inline ByteWriter &op_false();
	inline ByteWriter &op_true();
	inline ByteWriter &op_null();
	inline ByteWriter &op_empty_object();
	inline ByteWriter &op_begin_object();
	inline ByteWriter &op_end_object();

	inline ByteWriter &op(uint8_t v);
	inline ByteWriter &op(uint16_t v);
	inline ByteWriter &op(uint32_t v);
	inline ByteWriter &op(uint64_t v);
	inline ByteWriter &op(int8_t v);
	inline ByteWriter &op(int16_t v);
	inline ByteWriter &op(int32_t v);
	inline ByteWriter &op(int64_t v);
	inline ByteWriter &op(char v);

	inline ByteWriter &op_half(float value);
	inline ByteWriter &op(float value);
	inline ByteWriter &op(double value);
	
	inline ByteWriter &op_map_header(uint32_t elements);

public:
	inline ByteWriter &op_untyped_var_uint(uint64_t value)
	{
		const uint32_t bits = std::bit_width(value);
		uint32_t bytes = bits / 7;
		if (bytes == 0) {
			constexpr uint8_t byte = 0;
			_append(&byte, 1);
		} else {
			constexpr uint8_t masks[16] = {0x00, 0x7F, 0x3F, 0x1F, 0x0F,
										   0x07, 0x03, 0x01, 0x00, 0x00};
			constexpr uint8_t ones[16] = {0x00, 0x00, 0x80, 0xC0, 0xE0,
										  0xF0, 0xF8, 0xFC, 0xFE, 0xFF};
			constexpr uint8_t shifts[16] = {0, 7, 6, 5, 4, 3, 2, 1, 0, 0};

			const uint32_t offset = _expand(bytes);
			
			const uint8_t header = (masks[bytes] & value) | ones[bytes];
			ptr[offset] = header;
			
			value >>= shifts[bytes];
			value = HostToNetworkUint(value);
			memcpy(ptr+offset+1, &value, bytes-1);
		}
		return *this;
	}

	inline ByteWriter &op_untyped_var_int(int64_t value)
	{
		const uint64_t uvalue = value;
		const uint64_t sign = uvalue >> 63;
		const uint64_t abs = sign ? ~uvalue : uvalue;
		return op_untyped_var_int((abs << 1) | sign);
	}

public:
	inline ByteWriter &op_array_header(uint32_t elements)
	{
		_reserve_expand(10);
		if (elements == 0) { // empty array
			_append_byte(0b11001011);
		} else if (elements <= 16) { // size embeded in header
			_append_byte(0b11010000 | (elements - 1));
		} else { // size in following VAR_INT+17
			_append_byte(0b11001100);
			op_untyped_var_uint(elements - 17);
		}
		return *this;
	}

	template <typename T>
	inline ByteWriter &op(const T *data, uint32_t elements)
	{
		_reserve_expand((1 + sizeof(T)) * elements + 16);
		op_array_header(elements);
		for (uint32_t i = 0; i < elements; ++i)
			op(data[i]);
		return *this;
	}

	template <typename T> inline ByteWriter &op(const std::vector<T> &arr)
	{
		return op<T>(arr.data(), arr.size());
	}

public:
	inline void _append_byte(const uint8_t byte)
	{
		uint32_t oldSize = _expand(1);
		ptr[oldSize] = byte;
	}
	
	inline void _append(const uint8_t *data, uint32_t bytes)
	{
		uint32_t oldSize = _expand(bytes);
		memcpy(ptr + oldSize, data, bytes);
	}

private:
	inline size_t _expand(size_t bytesToExpand)
	{
		size_t oldSize = _buffer->size();
		_buffer->resize(oldSize + bytesToExpand);
		ptr = _buffer->data();
		return oldSize;
	}

	inline void _reserve_expand(size_t bytesToExpand)
	{
		_reserve(_buffer->size() + bytesToExpand);
	}

	inline void _reserve(size_t newCapacity)
	{
		_buffer->reserve((newCapacity * 3) / 2 + 16);
		ptr = _buffer->data();
	}

	BT *_buffer;
	uint8_t *ptr;

public:
	bool hasError = false;
};

template <typename BT>
inline ByteWriter<BT> &ByteWriter<BT>::op_sized_string(const std::string &str)
{
	return op_sized_string(str.data(), str.size());
}

template <typename BT>
inline ByteWriter<BT> &ByteWriter<BT>::op_sized_string(const std::string_view str)
{
	return op_sized_string(str.data(), str.size());
}

template <typename BT>
inline ByteWriter<BT> &ByteWriter<BT>::op_sized_string(const char *str, uint32_t size)
{
	uint32_t offset = _expand(size + 1);
	memcpy(ptr + offset, str, size);
	ptr[offset + size] = 0;
	return *this;
}

template <typename BT> inline ByteWriter<BT> &ByteWriter<BT>::op_sized_string(const char *str)
{
	ssize_t len = strlen(str);
	return op_sized_string(str, len);
}

template <typename BT>
inline ByteWriter<BT> &ByteWriter<BT>::op_cstring(const char *str, uint32_t size)
{
	return op_cstring((char *)str, size);
}

template <typename BT>
inline ByteWriter<BT> &ByteWriter<BT>::op_cstring(const char *str)
{
	return op((char *)str);
}

template <typename BT> inline ByteWriter<BT> &ByteWriter<BT>::op(uint8_t v)
{
	size_t offset = _expand(sizeof(v));
	ptr[offset] = v;
	++offset;
	return *this;
}
template <typename BT> inline ByteWriter<BT> &ByteWriter<BT>::op(uint16_t v)
{
	size_t offset = _expand(sizeof(v));
	v = HostToNetworkUint(v);
	memcpy(ptr + offset, &v, sizeof(v));
	return *this;
}
template <typename BT> inline ByteWriter<BT> &ByteWriter<BT>::op(uint32_t v)
{
	size_t offset = _expand(sizeof(v));
	v = HostToNetworkUint(v);
	memcpy(ptr + offset, &v, sizeof(v));
	return *this;
}
template <typename BT> inline ByteWriter<BT> &ByteWriter<BT>::op(uint64_t v)
{
	size_t offset = _expand(sizeof(v));
	v = HostToNetworkUint(v);
	memcpy(ptr + offset, &v, sizeof(v));
	return *this;
}

template <typename BT> inline ByteWriter<BT> &ByteWriter<BT>::op(bool v)
{
	return op(v ? (uint8_t)1 : (uint8_t)0);
}
template <typename BT> inline ByteWriter<BT> &ByteWriter<BT>::op(int8_t v)
{
	return op((uint8_t)v);
}
template <typename BT> inline ByteWriter<BT> &ByteWriter<BT>::op(int16_t v)
{
	return op((uint16_t)v);
}
template <typename BT> inline ByteWriter<BT> &ByteWriter<BT>::op(int32_t v)
{
	return op((uint32_t)v);
}
template <typename BT> inline ByteWriter<BT> &ByteWriter<BT>::op(int64_t v)
{
	return op((uint64_t)v);
}
template <typename BT> inline ByteWriter<BT> &ByteWriter<BT>::op(char v)
{
	return op((uint8_t)v);
}

template <typename BT> inline ByteWriter<BT> &ByteWriter<BT>::op_half(float value)
{
}

template <typename BT> inline ByteWriter<BT> &ByteWriter<BT>::op(float value)
{
	return op(*(uint32_t *)&value);
}

template <typename BT> inline ByteWriter<BT> &ByteWriter<BT>::op(double value)
{
	return op(*(uint64_t *)&value);
}

} // namespace v2
} // namespace bitscpp

#endif
