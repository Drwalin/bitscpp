// Copyright (C) 2023-2026 Marek Zalewski aka Drwalin
//
// This file is part of bitscpp project under MIT License // You should have
// received a copy of the MIT License along with this program.

#ifndef BITSCPP_BYTE_WRITER_V2_HPP
#define BITSCPP_BYTE_WRITER_V2_HPP

#include <cstdint>
#include <cstring>
#include <cassert>

#include <string>
#include <string_view>
#include <vector>

#include "V2_Specification.hpp"
#include "SerizalizerClass.hpp"

/*
 * Type BT requires following interface:
 * class BT {
 *   uint8_t *data();
 *   size_t size();
 *   void resize(size_t newSize);
 *   size_t capacity();
 *   void reserve(size_t newCapacity);
 *   void push_back(uint8_t byte);
 *   void write(const uint8_t *data, uint32_t bytes);
 * };
 *
 * behavior should be similar to std::vector<uint8_t>
 */

namespace bitscpp
{
namespace v2
{
template<typename BT>
class ByteWriter
{
public:
	constexpr static int VERSION = 2;
	constexpr static bool READER = false;
	constexpr static bool WRITER = true;

	template<typename TT>
	inline constexpr ByteWriter &op(const TT &item)
	{
		using T = std::remove_cvref_t<decltype(item)>;
		if constexpr (requires { item.serialize(*this); }) {
			item.serialize(*this);
		} else if constexpr (requires { ((T &)item).serialize(*this); }) {
			// TODO: remove this branch
			static_assert(!"Consider removing this branch");
			((T &)item).serialize(*this);
		} else if constexpr (requires {
				bitscpp::serializer<ByteWriter<BT>, T>::op(*this, item);
							 }) {
			bitscpp::serializer<ByteWriter<BT>, T>::op(*this, item);
		} else if constexpr (requires {
								 bitscpp::serializer<ByteWriter<BT>, T>::op(*this,
															   (T &)item);
							 }) {
			bitscpp::serializer<ByteWriter<BT>, T>::op(*this, (T &)item);
		} else if constexpr (requires { serialize(*this, item); }) {
			serialize(*this, item);
		} else if constexpr (requires { serialize(*this, *(T *)&item); }) {
			// TODO: remove this branch
			static_assert(!"Consider removing this branch");
			serialize(*this, *(T *)&item);
		} else {
			static_assert(
				!"Unimplemented bitscpp serialization function or method");
		}
		return *this;
	}

public:
	inline uint32_t GetSize() const
	{
		if (_buffer == nullptr)
			return 0;
		return _buffer->size();
	}

	inline void Init(BT *buffer) { _buffer = buffer; }
	inline ByteWriter() { Init(nullptr); }
	inline ByteWriter(BT *buffer) { Init(buffer); }

	// strings
	ByteWriter &op_sized_byte_array_header(uint32_t bytes);
	ByteWriter &op_sized_string_header(uint32_t bytes);

	ByteWriter &op(const std::string &str);
	ByteWriter &op(const std::string_view str);
	ByteWriter &op(char const *str);
	ByteWriter &op(char const *str, uint32_t size);
	ByteWriter &op(char *str);
	ByteWriter &op(char *str, uint32_t size);

	// constant size byte array
	ByteWriter &op_byte_array(const uint8_t *data, uint32_t bytes);
	ByteWriter &op_byte_array(const std::vector<uint8_t> &data);
	ByteWriter &op(const std::vector<uint8_t> &data);
	ByteWriter &op_byte_array(const std::vector<char> &data);
	ByteWriter &op(const std::vector<char> &data);

	// miscelanous
	ByteWriter &op(bool v);
	ByteWriter &op_boolean(bool v);
	ByteWriter &op_false();
	ByteWriter &op_true();
	ByteWriter &op_begin_object();
	ByteWriter &op_end_object();

	// integers
	ByteWriter &op(uint8_t v);
	ByteWriter &op(uint16_t v);
	ByteWriter &op(uint32_t v);
	ByteWriter &op(uint64_t v);
	ByteWriter &op(int8_t v);
	ByteWriter &op(int16_t v);
	ByteWriter &op(int32_t v);
	ByteWriter &op(int64_t v);
	ByteWriter &op(char v);

	ByteWriter &op_uint(uint64_t v);
	ByteWriter &op_int(int64_t v);

	// floats
	ByteWriter &op_half(float value);
	ByteWriter &op_bfloat(float value);
	ByteWriter &op_float(float value);
	ByteWriter &op_double(double value);
	ByteWriter &op(float value);
	ByteWriter &op(double value);

	// map
	ByteWriter &op_map_header(uint32_t elements);

	// array
	ByteWriter &op_array_header(uint32_t elements);

	ByteWriter &op_untyped_var_uint(uint64_t value);
	ByteWriter &op_untyped_var_int(int64_t value);

	ByteWriter &op_untyped_uint32(uint32_t value);

	void set_error(Errors error);
	Errors get_errors() const;

public:
	template <typename T>
	inline ByteWriter &op(const T *data, uint32_t elements)
	{
		assert(elements <= MAX_ARRAY_ELEMENTS);
		if (elements > MAX_ARRAY_ELEMENTS) {
			set_error(ERROR_ARRAY_TOO_BIG);
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
			set_error(ERROR_ARRAY_TOO_BIG);
			return *this;
		}
		return op<T>(arr.data(), arr.size());
	}

private:
	void _append_byte(const uint8_t byte);
	void _append(const uint8_t *data, uint32_t bytes);

private:
	uint8_t *_expand(size_t bytesToExpand);
	void _reserve_expand(size_t bytesToExpand);
	void _reserve(size_t newCapacity);

public:
	BT *_buffer = nullptr;
	uint32_t errors = 0;
};

} // namespace v2
} // namespace bitscpp

#endif
