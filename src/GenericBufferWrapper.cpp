// Copyright (C) 2026 Marek Zalewski aka Drwalin
//
// This file is part of bitscpp project under MIT License
// You should have received a copy of the MIT License along with this program.

#pragma once

#ifndef BITSCPP_GENERIC_BUFFER_WRAPPER_CPP
#define BITSCPP_GENERIC_BUFFER_WRAPPER_CPP

#if not defined _BITSCPP_INLINE
#define BITSCPP_NO_INLINE_CPP_GENERIC_BUFFER_WRAPPER
#endif

#include <cassert>
#include <cstring>

#include "../include/bitscpp/GenericBufferWrapper.hpp"

#undef BITSCPP_INLINE
#if defined BITSCPP_NO_INLINE_CPP_GENERIC_BUFFER_WRAPPER
#define BITSCPP_INLINE
#else
#define BITSCPP_INLINE _BITSCPP_INLINE
#endif

namespace bitscpp
{
BITSCPP_INLINE
GenericBufferWriter::GenericBufferWriter(GenericBufferWriterPimpl impl)
	: impl(impl)
{
}

BITSCPP_INLINE
GenericBufferWriter::GenericBufferWriter(void *buffer,
										 GenericBufferWriterPimpl impl)
	: impl(impl), buffer(buffer)
{
}

BITSCPP_INLINE GenericBufferWriter::GenericBufferWriter(GenericBufferWriter &&o)
{
	impl = o.impl;
	buffer = o.buffer;
	head = o.head;
	end = o.end;
	o.impl = {};
	o.buffer = nullptr;
	o.head = nullptr;
	o.end = nullptr;
}

#if defined BITSCPP_NO_INLINE_CPP_GENERIC_BUFFER_WRAPPER
GenericBufferWriter &GenericBufferWriter::operator=(GenericBufferWriter &&o)
{
	if (buffer) {
		assert(impl.destructor);
		impl.destructor(this);
		buffer = nullptr;
		head = nullptr;
		end = nullptr;
	}
	impl = o.impl;
	buffer = o.buffer;
	head = o.head;
	end = o.end;
	o.impl = {};
	o.buffer = nullptr;
	o.head = nullptr;
	o.end = nullptr;
	return *this;
}
#endif

BITSCPP_INLINE GenericBufferWriter::~GenericBufferWriter()
{
	if (buffer) {
		assert(impl.destructor);
		impl.destructor(this);
		buffer = nullptr;
		head = nullptr;
		end = nullptr;
	}
}

BITSCPP_INLINE void *GenericBufferWriter::release_buffer()
{
	if (buffer == nullptr) {
		return nullptr;
	}
	assert(impl.release_underlying_buffer);
	void *const buf = buffer;
	impl.apply_head_end_to_underlying(this);
	buffer = nullptr;
	head = nullptr;
	end = nullptr;
	return buf;
}

BITSCPP_INLINE void
GenericBufferWriter::append_byte(const uint8_t byte) noexcept
{
	if (head == end) [[unlikely]] {
		expand(1)[0] = byte;
	} else {
		*head = byte;
		head++;
	}
}

BITSCPP_INLINE void GenericBufferWriter::append(uint8_t const *data,
												size_t bytes) noexcept
{
	size_t available = end - head;
	if (available >= bytes) [[likely]] {
		memcpy(head, data, bytes);
		head += bytes;
		return;
	}
	append_slow(data, bytes);
}

#if defined BITSCPP_NO_INLINE_CPP_GENERIC_BUFFER_WRAPPER
void GenericBufferWriter::append_slow(uint8_t const *data,
									  size_t bytes) noexcept
{
	const uint8_t *_end = data + bytes;
	while (const size_t to_write = (_end - data)) {
		const size_t available = end - head;

		if (to_write <= available) {
			memcpy(head, data, to_write);
			head += to_write;
			return;
		}
		memcpy(head, data, available);
		data += available;
		const size_t left = (_end - data);

		impl.expand(this, 1, left);
	}
}
#endif

BITSCPP_INLINE uint8_t *GenericBufferWriter::expand(uint8_t bytes) noexcept
{
	uint8_t *ret = head;
	head += bytes;
	if (head > end) [[unlikely]] {
		head -= bytes;
		impl.expand(this, bytes, bytes);
		ret = head;
		head += bytes;
	}
	return ret;
}

BITSCPP_INLINE void
GenericBufferWriter::expand_hint(size_t hint_bytes_to_expand) noexcept
{
	if ((head + hint_bytes_to_expand) > end) [[unlikely]] {
		impl.expand(this, 1, hint_bytes_to_expand);
	}
}
} // namespace bitscpp

#endif
