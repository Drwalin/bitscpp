// Copyright (C) 2023-2026 Marek Zalewski aka Drwalin
//
// This file is part of bitscpp project under MIT License
// You should have received a copy of the MIT License along with this program.

#ifndef BITSCPP_BYTE_READER_V2_HPP
#define BITSCPP_BYTE_READER_V2_HPP

#include <cstdint>

#include <string>
#include <string_view>
#include <vector>

#include "V2_Specification.hpp"

namespace bitscpp
{
namespace v2
{

class ByteReader;

template <typename T> inline ByteReader &op(ByteReader &reader, T &data)
{
	data.__ByteStream_op(reader);
	return reader;
}

namespace impl
{
template <typename T>
static inline ByteReader &__op_ptr(ByteReader &reader, T *data)
{
	op(reader, *data);
	return reader;
}

template <typename T>
static inline ByteReader &__op_ref(ByteReader &reader, T &data)
{
	op(reader, data);
	return reader;
}
} // namespace impl

enum ByteReaderErrors : uint32_t {
	ERROR_OK = 0,
	ERROR_BUFFER_TOO_SMALL = 1,
	ERROR_TYPE_MISMATCH = 2,
};

class ByteReader
{
public:
	template <typename T> inline ByteReader &op(T *data)
	{
		impl::__op_ptr(*this, data);
		return *this;
	}

	template <typename T> inline ByteReader &op(T &data)
	{
		impl::__op_ref(*this, data);
		return *this;
	}

public:
	inline ByteReader(const uint8_t *buffer, uint32_t offset, uint32_t size)
		: _buffer(buffer), total_size(size), ptr(buffer + offset),
		  end(buffer + size), errors(0)
	{
	}
	inline ByteReader(const uint8_t *buffer, uint32_t size)
		: _buffer(buffer), total_size(size), ptr(buffer), end(buffer + size),
		  errors(0)
	{
	}

public:
	Type get_next_type() const;
	Type get_next_detailed_type() const;

	bool is_next_integer() const;
	bool is_next_floating_point() const;
	bool is_next_float16() const;
	bool is_next_bfloat16() const;
	bool is_next_float32() const;
	bool is_next_float64() const;
	bool is_next_bool() const;
	bool is_next_string() const;
	bool is_next_cstring() const;
	bool is_next_map() const;
	bool is_next_array() const;
	bool is_next_beg_object() const;
	bool is_next_end_object() const;

public:
	// strings
	ByteReader &op_sized_byte_array_header(uint32_t &bytes);
	ByteReader &op_sized_string_header(uint32_t &bytes);
	ByteReader &op_cstring_header(uint32_t &bytes);

	ByteReader &op_sized_string(std::string &str);
	ByteReader &op_sized_string(std::string_view &str);
	ByteReader &op_sized_string(char const *&str, uint32_t &size);
	ByteReader &op_cstring(const char *&str);
	ByteReader &op_cstring(const char *&str, uint32_t &size);

	// byte array
	ByteReader &op_byte_array(uint8_t const *&data, uint32_t &bytes);
	ByteReader &op(std::vector<uint8_t> &data);

	// miscelanous
	ByteReader &op(bool &v);
	ByteReader &op_boolean(bool &v);
	ByteReader &op_begin_object();
	ByteReader &op_end_object();

	// integers
	ByteReader &op(uint8_t &v);
	ByteReader &op(uint16_t &v);
	ByteReader &op(uint32_t &v);
	ByteReader &op(uint64_t &v);
	ByteReader &op(int8_t &v);
	ByteReader &op(int16_t &v);
	ByteReader &op(int32_t &v);
	ByteReader &op(int64_t &v);
	ByteReader &op(char &v);

	ByteReader &op_uint(uint64_t &v);
	ByteReader &op_int(int64_t &v);

	// floats
	ByteReader &op_half(float &v);
	ByteReader &op_bfloat(float &v);
	ByteReader &op_float(float &v);
	ByteReader &op_double(double &v);
private:
	static void _read_half(const uint8_t *ptr, float &v);
	static void _read_bfloat(const uint8_t *ptr, float &v);
	static void _read_float(const uint8_t *ptr, float &v);
	static void _read_double(const uint8_t *ptr, double &v);
public:
	ByteReader &op(float &v);
	ByteReader &op(double &v);

	// map
	ByteReader &op_map_header(uint32_t &elements);
	
	// array
	ByteReader &op_array_header(uint32_t &elements);

	ByteReader &op_untyped_var_uint(uint64_t &v);
	ByteReader &op_untyped_var_int(int64_t &v);

public:
	template <typename T, typename Te>
	inline ByteReader &op(T *data, Te elements)
	{
		for (uint32_t i = 0; i < elements; ++i)
			op(data[i]);
		return *this;
	}

	template <typename T> inline ByteReader &op(std::vector<T> &arr)
	{
		uint32_t elems;
		op(elems);
		arr.resize(elems);
		return op<T>(arr.data(), elems);
	}

	bool is_valid() const;
	uint32_t get_errors() const;
	bool has_any_more() const;
	uint32_t get_offset() const;
	const uint8_t *get_buffer() const;
	uint32_t get_remaining_bytes() const;

protected:
	bool has_bytes_to_read(uint32_t bytes) const;

	uint8_t const *_buffer;
	uint32_t total_size;

	uint8_t const *ptr;
	uint8_t const *end;

	uint32_t errors;
};

}
} // namespace bitscpp

#endif
