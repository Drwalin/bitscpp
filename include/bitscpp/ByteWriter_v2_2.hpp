// Copyright (C) 2023-2026 Marek Zalewski aka Drwalin
//
// This file is part of bitscpp project under MIT License
// You should have received a copy of the MIT License along with this program.

#ifndef BITSCPP_BYTE_WRITER_V2_2_HPP
#define BITSCPP_BYTE_WRITER_V2_2_HPP

#include <cstdint>
#include <cstring>
#include <cassert>

#include <string>
#include <string_view>
#include <vector>

#include "BitscppForward.hpp"
#include "V2_Specification.hpp"
#include "SerizalizerClass.hpp"
#include "GenericBufferWrapper.hpp"

namespace bitscpp
{
namespace v2_2
{
using v2::MAX_ARRAY_ELEMENTS;
using v2::MAX_BUFFER_SIZE;
using v2::Errors;
using v2::Type;
using v2::TypeHeader;
using v2::TypeHeader_ImmediateValues;
using v2::TypeHeader_ImmediateIntegerRanges;

class ByteWriter
{
public:
	inline constexpr static int VERSION = 2;
	inline constexpr static bool READER = false;
	inline constexpr static bool WRITER = true;

	template <typename TT> inline constexpr ByteWriter &op(const TT &item)
	{
		using T = std::remove_cvref_t<decltype(item)>;
		if constexpr (requires { item.serialize(*this); }) {
			item.serialize(*this);
		} else if constexpr (requires { ((T &)item).serialize(*this); }) {
			// TODO: remove this branch
			static_assert("Consider removing this branch");
			((T &)item).serialize(*this);
		} else if constexpr (requires {
								 bitscpp::serializer<ByteWriter, T>::op(*this,
																		item);
							 }) {
			bitscpp::serializer<ByteWriter, T>::op(*this, item);
		} else if constexpr (requires {
								 bitscpp::serializer<ByteWriter, T>::op(
									 *this, (T &)item);
							 }) {
			bitscpp::serializer<ByteWriter, T>::op(*this, (T &)item);
		} else if constexpr (requires { serialize(*this, item); }) {
			serialize(*this, item);
		} else if constexpr (requires { serialize(*this, *(T *)&item); }) {
			// TODO: remove this branch
			static_assert("Consider removing this branch");
			serialize(*this, *(T *)&item);
		} else {
			static_assert(
				!"Unimplemented bitscpp serialization function or method");
		}
		return *this;
	}

public:
	_BITSCPP_INLINE void Init(GenericBufferWriter &&buf);
	_BITSCPP_INLINE ByteWriter(GenericBufferWriterPimpl impl);
	_BITSCPP_INLINE ByteWriter(GenericBufferWriter &&buf);
	_BITSCPP_INLINE ByteWriter(ByteWriter &&o);
	_BITSCPP_INLINE ByteWriter &operator=(ByteWriter &&o);

	_BITSCPP_INLINE void *release_buffer_impl();

	_BITSCPP_INLINE ByteWriter(ByteWriter &o) = delete;
	_BITSCPP_INLINE ByteWriter(const ByteWriter &o) = delete;
	_BITSCPP_INLINE ByteWriter &operator=(ByteWriter &o) = delete;
	_BITSCPP_INLINE ByteWriter &operator=(const ByteWriter &o) = delete;

	// strings
	_BITSCPP_INLINE ByteWriter &op_sized_byte_array_header(uint32_t bytes);
	_BITSCPP_INLINE ByteWriter &op_sized_string_header(uint32_t bytes);

	_BITSCPP_INLINE ByteWriter &op(const std::string &str);
	_BITSCPP_INLINE ByteWriter &op(const std::string_view str);
	_BITSCPP_INLINE ByteWriter &op(char const *str);
	_BITSCPP_INLINE ByteWriter &op(char const *str, uint32_t size);
	_BITSCPP_INLINE ByteWriter &op(char *str);
	_BITSCPP_INLINE ByteWriter &op(char *str, uint32_t size);

	// constant size byte array
	_BITSCPP_INLINE ByteWriter &op_byte_array(const uint8_t *data,
											  uint32_t bytes);
	_BITSCPP_INLINE ByteWriter &op_byte_array(const std::vector<uint8_t> &data);
	_BITSCPP_INLINE ByteWriter &op(const std::vector<uint8_t> &data);
	_BITSCPP_INLINE ByteWriter &op_byte_array(const std::vector<char> &data);
	_BITSCPP_INLINE ByteWriter &op(const std::vector<char> &data);

	// miscelanous
	_BITSCPP_INLINE ByteWriter &op(bool v);
	_BITSCPP_INLINE ByteWriter &op_boolean(bool v);
	_BITSCPP_INLINE ByteWriter &op_false();
	_BITSCPP_INLINE ByteWriter &op_true();
	_BITSCPP_INLINE ByteWriter &op_begin_object();
	_BITSCPP_INLINE ByteWriter &op_end_object();

	// integers
	_BITSCPP_INLINE ByteWriter &op(uint8_t v);
	_BITSCPP_INLINE ByteWriter &op(uint16_t v);
	_BITSCPP_INLINE ByteWriter &op(uint32_t v);
	_BITSCPP_INLINE ByteWriter &op(uint64_t v);
	_BITSCPP_INLINE ByteWriter &op(int8_t v);
	_BITSCPP_INLINE ByteWriter &op(int16_t v);
	_BITSCPP_INLINE ByteWriter &op(int32_t v);
	_BITSCPP_INLINE ByteWriter &op(int64_t v);
	_BITSCPP_INLINE ByteWriter &op(char v);

	_BITSCPP_INLINE ByteWriter &op_uint(uint64_t v);
	_BITSCPP_INLINE ByteWriter &op_int(int64_t v);

	// floats
	_BITSCPP_INLINE ByteWriter &op_half(float value);
	_BITSCPP_INLINE ByteWriter &op_bfloat(float value);
	_BITSCPP_INLINE ByteWriter &op_float(float value);
	_BITSCPP_INLINE ByteWriter &op_double(double value);
	_BITSCPP_INLINE ByteWriter &op(float value);
	_BITSCPP_INLINE ByteWriter &op(double value);

	// map
	_BITSCPP_INLINE ByteWriter &op_map_header(uint32_t elements);

	// array
	_BITSCPP_INLINE ByteWriter &op_array_header(uint32_t elements);

	_BITSCPP_INLINE ByteWriter &op_untyped_var_uint(uint64_t value);
	_BITSCPP_INLINE ByteWriter &op_untyped_var_int(int64_t value);

	_BITSCPP_INLINE ByteWriter &op_untyped_uint32(uint32_t value);

	_BITSCPP_INLINE void set_error(Errors error);
	_BITSCPP_INLINE Errors get_errors() const;

public:
	template <typename T>
	inline ByteWriter &op(const T *data, uint32_t elements)
	{
		assert(elements <= MAX_ARRAY_ELEMENTS);
		if (elements > MAX_ARRAY_ELEMENTS) {
			set_error(Errors::ERROR_ARRAY_TOO_BIG);
			return *this;
		}
		_reserve_expand(32 + sizeof(T) * elements);
		op_array_header(elements);
		for (uint32_t i = 0; i < elements; ++i)
			op(data[i]);
		return *this;
	}

	template <typename T> inline ByteWriter &op(const std::vector<T> &arr)
	{
		assert(arr.size() <= MAX_ARRAY_ELEMENTS);
		if (arr.size() > MAX_ARRAY_ELEMENTS) {
			set_error(Errors::ERROR_ARRAY_TOO_BIG);
			return *this;
		}
		return op<T>(arr.data(), arr.size());
	}

private:
	_BITSCPP_INLINE void _append_byte(const uint8_t byte);
	_BITSCPP_INLINE void _append(const uint8_t *data, uint32_t bytes);

private:
	_BITSCPP_INLINE uint8_t *_expand(uint8_t bytesToExpand);
	_BITSCPP_INLINE void _reserve_expand(size_t bytesToExpand);

private:
	GenericBufferWriter _buffer;
	uint32_t errors = 0;
};

} // namespace v2_2
} // namespace bitscpp

#endif

#ifdef BITSCPP_ENABLE_INLINE_V2
#include "../../src/ByteWriter_v2_2.cpp"
#endif
