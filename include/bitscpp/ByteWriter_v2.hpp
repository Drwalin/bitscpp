// Copyright (C) 2023-2026 Marek Zalewski aka Drwalin
//
// This file is part of bitscpp project under MIT License
// You should have received a copy of the MIT License along with this program.

#ifndef BITSCPP_BYTE_WRITER_V2_HPP
#define BITSCPP_BYTE_WRITER_V2_HPP

#include <cstdint>
#include <cstring>

#include <string>
#include <string_view>
#include <vector>

/*
 * BT_TYPE requires following interface:
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

#ifndef BITSCPP_BYTE_WRITER_V2_BT_TYPE
#define BITSCPP_BYTE_WRITER_V2_BT_TYPE std::vector<uint8_t>
#endif

#ifndef BITSCPP_BYTE_WRITER_V2_NAME_SUFFIX
#define BITSCPP_BYTE_WRITER_V2_NAME_SUFFIX _vector
#endif

#define BITSCPP_CONCATENATE_NAMES_(LEFT, RIGHT) LEFT##RIGHT
#define BITSCPP_CONCATENATE_NAMES(LEFT, RIGHT)                                 \
	BITSCPP_CONCATENATE_NAMES_(LEFT, RIGHT)

#define ByteWriter                                                             \
	BITSCPP_CONCATENATE_NAMES(ByteWriter, BITSCPP_BYTE_WRITER_V2_NAME_SUFFIX)

namespace bitscpp
{
namespace v2
{
class ByteWriter;
}

template <typename T> struct _impl_v2_writer {
	static inline v2::ByteWriter &op(v2::ByteWriter &writer, const T &data)
	{
		((T &)data).__ByteStream_op(writer);
		return writer;
	}
};

namespace v2
{
namespace impl
{
template <typename T>
static inline ByteWriter &__op_ptr(v2::ByteWriter &writer, T *const data)
{
	return bitscpp::_impl_v2_writer<T>::op(writer, *data);
}

template <typename T>
static inline ByteWriter &__op_ref(v2::ByteWriter &writer, const T &data)
{
	return bitscpp::_impl_v2_writer<T>::op(writer, data);
}
} // namespace impl

class ByteWriter
{
public:
	constexpr static int VERSION = 2;

	using BT = BITSCPP_BYTE_WRITER_V2_BT_TYPE;

	template <typename T> inline ByteWriter &op(T *const data)
	{
		return bitscpp::_impl_v2_writer<T>::op(*this, *data);
		impl::__op_ptr(*this, data);
		return *this;
	}

	template <typename T> inline ByteWriter &op(const T &data)
	{
		return bitscpp::_impl_v2_writer<T>::op(*this, data);
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
	ByteWriter &op_sized_byte_array_header(uint32_t bytes);
	ByteWriter &op_sized_string_header(uint32_t bytes);
	ByteWriter &op_cstring_header();

	ByteWriter &op_sized_string(const std::string &str);
	ByteWriter &op_sized_string(const std::string_view str);
	ByteWriter &op_sized_string(char const *str);
	ByteWriter &op_sized_string(char const *str, uint32_t size);
	ByteWriter &op_cstring(const std::string &str);
	ByteWriter &op_cstring(const std::string_view str);
	ByteWriter &op_cstring(char const *str);
	ByteWriter &op_cstring(char const *str, uint32_t size);
	ByteWriter &op(const std::string &str);
	ByteWriter &op(const std::string_view str);
	ByteWriter &op(const char *str);

	// constant size byte array
	ByteWriter &op_byte_array(const uint8_t *data, uint32_t bytes);
	ByteWriter &op_byte_array(const std::vector<uint8_t> &data);
	ByteWriter &op(const std::vector<uint8_t> &data);

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

public:
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

private:
	void _append_byte(const uint8_t byte);
	void _append(const uint8_t *data, uint32_t bytes);

private:
	size_t _expand(size_t bytesToExpand);
	void _reserve_expand(size_t bytesToExpand);
	void _reserve(size_t newCapacity);

private:
	BT *_buffer;
	uint8_t *ptr;

public:
	bool hasError = false;
};

} // namespace v2
} // namespace bitscpp

#undef ByteWriter

#endif
