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
#include "SerizalizerClass.hpp"

namespace bitscpp
{
namespace v2
{
class ByteReader
{
public:
	constexpr static int VERSION = 2;
	constexpr static bool READER = true;
	constexpr static bool WRITER = false;

	template<typename TT>
	constexpr ByteReader &op(TT &item)
	{
		using T = std::remove_cvref_t<decltype(item)>;
		if constexpr (requires { item.serialize(*this); }) {
			item.serialize(*this);
		} else if constexpr (requires {
								 serializer<ByteReader, T>::op(*this, item);
							 }) {
			serializer<ByteReader, T>::op(*this, item);
		} else if constexpr (requires { serialize(*this, item); }) {
			serialize(*this, (T &)item);
		} else {
			static_assert(
				!"unimplemented bitscpp deserialization function or method");
		}
		return *this;
	}

public:
	inline ByteReader(const uint8_t *buffer, uint32_t offset, uint32_t size)
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
	inline ByteReader(const uint8_t *buffer, uint32_t size)
		: ByteReader(buffer, 0, size)
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
	bool is_next_map() const;
	bool is_next_array() const;
	bool is_next_beg_object() const;
	bool is_next_end_object() const;

public:
	// strings
	ByteReader &op_sized_byte_array_header(uint32_t &bytes);
	ByteReader &op_sized_string_header(uint32_t &bytes);

	ByteReader &op(std::string_view &str);
	ByteReader &op_byte_array(char const **str, uint32_t &size);
	ByteReader &op(char const *&str, uint32_t &size);
	ByteReader &op(std::string &str);

	// byte array
	ByteReader &op_byte_array(uint8_t const **data, uint32_t &bytes);
	ByteReader &op_byte_array(uint8_t const *&data, uint32_t &bytes);
	ByteReader &op_byte_array(std::vector<uint8_t> &data);
	ByteReader &op(std::vector<uint8_t> &data);
	ByteReader &op_byte_array(std::vector<char> &data);
	ByteReader &op(std::vector<char> &data);

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
	static void _read_half(const uint8_t *ptr, float &v);
	static void _read_bfloat(const uint8_t *ptr, float &v);
	static void _read_float(const uint8_t *ptr, float &v);
	static void _read_double(const uint8_t *ptr, double &v);
	ByteReader &op(float &v);
	ByteReader &op(double &v);

	// map
	ByteReader &op_map_header(uint32_t &elements);

	// array
	ByteReader &op_array_header(uint32_t &elements);

	ByteReader &op_untyped_var_uint(uint64_t &v);
	ByteReader &op_untyped_var_int(int64_t &v);

	ByteReader &op_untyped_uint32(uint32_t &v);

	// util
	ByteReader &skip(uint32_t bytes);

public:
	template <typename T>
	inline ByteReader &op(T *data, const uint32_t elements)
	{
		uint32_t size = 0;
		op_array_header(size);
		if (get_errors()) {
			[[unlikely]];
			return *this;
		}
		if (size != elements) {
			[[unlikely]];
			set_error(ERROR_TYPE_MISMATCH);
			return *this;
		}
		if (elements > v2::MAX_ARRAY_ELEMENTS) {
			[[unlikely]];
			set_error(ERROR_ARRAY_TOO_BIG);
			set_error(ERROR_TYPE_MISMATCH);
			set_error(ERROR_BUFFER_TOO_SMALL);
			return *this;
		}
		for (uint32_t i = 0; i < elements && errors == 0; ++i)
			op(data[i]);
		return *this;
	}

	template <typename T> inline ByteReader &op(std::vector<T> &arr)
	{
		uint32_t elems = 0;
		op_array_header(elems);
		if (get_errors()) {
			[[unlikely]];
			return *this;
		}
		if (elems > v2::MAX_ARRAY_ELEMENTS) {
			[[unlikely]];
			set_error(ERROR_ARRAY_TOO_BIG);
			return *this;
		}
		if (elems > get_remaining_bytes()) {
			[[unlikely]];
			set_error(ERROR_ARRAY_TOO_BIG);
			set_error(ERROR_TYPE_MISMATCH);
			set_error(ERROR_BUFFER_TOO_SMALL);
			return *this;
		}
		arr.resize(elems);
		for (uint32_t i = 0; i < elems && errors == 0; ++i)
			op(arr[i]);
		return *this;
	}

	bool is_valid() const;
	Errors get_errors() const;
	bool has_any_more() const;
	uint32_t get_offset() const;
	const uint8_t *get_buffer() const;
	uint32_t get_remaining_bytes() const;

	void set_error(Errors error);

protected:
	bool has_bytes_to_read(uint32_t bytes) const;

	uint8_t const *_buffer = nullptr;

	uint8_t const *ptr = nullptr;
	uint8_t const *end = nullptr;

	uint32_t total_size = 0;
	uint32_t errors = 0;
};

} // namespace v2
} // namespace bitscpp

#endif
