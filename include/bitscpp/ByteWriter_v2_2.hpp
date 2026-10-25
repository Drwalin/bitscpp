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

#if defined BITSCPP_INLINE_BOOL
#  warning INLINE ............ 
# else
#  warning NO INLINE ............
# endif

namespace bitscpp
{
namespace v2_2
{
inline const static uint32_t MAX_ARRAY_ELEMENTS = v2::MAX_ARRAY_ELEMENTS;
inline const static uint32_t MAX_BUFFER_SIZE = v2::MAX_BUFFER_SIZE;
using Errors = v2::Errors;
using Type = v2::Type;
using TypeHeader = v2::TypeHeader;
using TypeHeader_ImmediateValues = v2::TypeHeader_ImmediateValues;
using TypeHeader_ImmediateIntegerRanges = v2::TypeHeader_ImmediateIntegerRanges;

class ByteWriter
{
public:
	inline constexpr static int VERSION = 2;
	inline constexpr static bool READER = false;
	inline constexpr static bool WRITER = true;

	template<typename TT>
	inline constexpr ByteWriter &op(const TT &item)
	{
		using T = std::remove_cvref_t<decltype(item)>;
		if constexpr (requires { item.serialize(*this); }) {
			item.serialize(*this);
		} else if constexpr (requires { ((T &)item).serialize(*this); }) {
			// TODO: remove this branch
			static_assert("Consider removing this branch");
			((T &)item).serialize(*this);
		} else if constexpr (requires {
				bitscpp::serializer<ByteWriter, T>::op(*this, item);
							 }) {
			bitscpp::serializer<ByteWriter, T>::op(*this, item);
		} else if constexpr (requires {
								 bitscpp::serializer<ByteWriter, T>::op(*this,
															   (T &)item);
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
	BITSCPP_INLINE void Init(GenericBufferWriter &&buf);
	BITSCPP_INLINE ByteWriter(GenericBufferWriterPimpl impl);
	BITSCPP_INLINE ByteWriter(GenericBufferWriter &&buf);
	BITSCPP_INLINE ByteWriter(ByteWriter &&o);
	BITSCPP_INLINE ByteWriter &operator=(ByteWriter &&o);

	BITSCPP_INLINE void *release_buffer_impl();

	BITSCPP_INLINE ByteWriter(ByteWriter &o) = delete;
	BITSCPP_INLINE ByteWriter(const ByteWriter &o) = delete;
	BITSCPP_INLINE ByteWriter &operator=(ByteWriter &o) = delete;
	BITSCPP_INLINE ByteWriter &operator=(const ByteWriter &o) = delete;

	// strings
	BITSCPP_INLINE ByteWriter &op_sized_byte_array_header(uint32_t bytes);
	BITSCPP_INLINE ByteWriter &op_sized_string_header(uint32_t bytes);

	BITSCPP_INLINE ByteWriter &op(const std::string &str);
	BITSCPP_INLINE ByteWriter &op(const std::string_view str);
	BITSCPP_INLINE ByteWriter &op(char const *str);
	BITSCPP_INLINE ByteWriter &op(char const *str, uint32_t size);
	BITSCPP_INLINE ByteWriter &op(char *str);
	BITSCPP_INLINE ByteWriter &op(char *str, uint32_t size);

	// constant size byte array
	BITSCPP_INLINE ByteWriter &op_byte_array(const uint8_t *data, uint32_t bytes);
	BITSCPP_INLINE ByteWriter &op_byte_array(const std::vector<uint8_t> &data);
	BITSCPP_INLINE ByteWriter &op(const std::vector<uint8_t> &data);
	BITSCPP_INLINE ByteWriter &op_byte_array(const std::vector<char> &data);
	BITSCPP_INLINE ByteWriter &op(const std::vector<char> &data);

	// miscelanous
	BITSCPP_INLINE ByteWriter &op(bool v);
	BITSCPP_INLINE ByteWriter &op_boolean(bool v);
	BITSCPP_INLINE ByteWriter &op_false();
	BITSCPP_INLINE ByteWriter &op_true();
	BITSCPP_INLINE ByteWriter &op_begin_object();
	BITSCPP_INLINE ByteWriter &op_end_object();

	// integers
	BITSCPP_INLINE ByteWriter &op(uint8_t v);
	BITSCPP_INLINE ByteWriter &op(uint16_t v);
	BITSCPP_INLINE ByteWriter &op(uint32_t v);
	BITSCPP_INLINE ByteWriter &op(uint64_t v);
	BITSCPP_INLINE ByteWriter &op(int8_t v);
	BITSCPP_INLINE ByteWriter &op(int16_t v);
	BITSCPP_INLINE ByteWriter &op(int32_t v);
	BITSCPP_INLINE ByteWriter &op(int64_t v);
	BITSCPP_INLINE ByteWriter &op(char v);

	BITSCPP_INLINE ByteWriter &op_uint(uint64_t v);
	BITSCPP_INLINE ByteWriter &op_int(int64_t v);

	// floats
	BITSCPP_INLINE ByteWriter &op_half(float value);
	BITSCPP_INLINE ByteWriter &op_bfloat(float value);
	BITSCPP_INLINE ByteWriter &op_float(float value);
	BITSCPP_INLINE ByteWriter &op_double(double value);
	BITSCPP_INLINE ByteWriter &op(float value);
	BITSCPP_INLINE ByteWriter &op(double value);

	// map
	BITSCPP_INLINE ByteWriter &op_map_header(uint32_t elements);

	// array
	BITSCPP_INLINE ByteWriter &op_array_header(uint32_t elements);

	BITSCPP_INLINE ByteWriter &op_untyped_var_uint(uint64_t value);
	BITSCPP_INLINE ByteWriter &op_untyped_var_int(int64_t value);

	BITSCPP_INLINE ByteWriter &op_untyped_uint32(uint32_t value);

	BITSCPP_INLINE void set_error(Errors error);
	BITSCPP_INLINE Errors get_errors() const;

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
	BITSCPP_INLINE void _append_byte(const uint8_t byte);
	BITSCPP_INLINE void _append(const uint8_t *data, uint32_t bytes);

private:
	BITSCPP_INLINE uint8_t *_expand(uint8_t bytesToExpand);
	BITSCPP_INLINE void _reserve_expand(size_t bytesToExpand);

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
