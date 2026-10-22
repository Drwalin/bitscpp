// Copyright (C) 2026 Marek Zalewski aka Drwalin
//
// This file is part of bitscpp project under MIT License
// You should have received a copy of the MIT License along with this program.

#include <cassert>

#include "../include/bitscpp/GenericBufferWrapper.hpp"

namespace bitscpp
{
GenericBufferWriter::GenericBufferWriter(GenericBufferWriterImpl impl)
	: impl(impl)
{
}

GenericBufferWriter::GenericBufferWriter(void *buffer,
										 GenericBufferWriterImpl impl)
	: impl(impl), buffer(buffer)
{
}

GenericBufferWriter::GenericBufferWriter(GenericBufferWriter &&o)
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

GenericBufferWriter::~GenericBufferWriter()
{
	if (buffer) {
		assert(impl.destructor);
		impl.destructor(this);
		buffer = nullptr;
		head = nullptr;
		end = nullptr;
	}
}

void *GenericBufferWriter::release_buffer()
{
	if (buffer == nullptr) {
		return nullptr;
	}
	assert(impl.release_underlying_buffer);
	void *const buf = buffer;
	impl.release_underlying_buffer(this);
	buffer = nullptr;
	head = nullptr;
	end = nullptr;
	return buf;
}

void GenericBufferWriter::append_byte(const uint8_t byte) noexcept
{
	if (head == end) {
		expand(1)[0] = byte;
	} else {
		*head = byte;
		head++;
	}
}

void GenericBufferWriter::append(uint8_t const *data, size_t bytes) noexcept
{
	size_t available = end - head;
	if (available >= bytes) {
		[[likely]];
		memcpy(head, data, bytes);
		head += bytes;
		return;
	}
	append_slow(data, bytes);
}

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

		impl.expand_hint(this, left);
	}
}

uint8_t *GenericBufferWriter::expand(uint8_t bytes) noexcept
{
	if ((head + bytes) > end) {
		[[unlikely]];
		impl.expand(this, bytes);
	}
	uint8_t *ret = head;
	head += bytes;
	return ret;
}

void GenericBufferWriter::expand_hint(size_t hint_bytes_to_expand) noexcept
{
	if ((head + hint_bytes_to_expand) > end) {
		[[unlikely]];
		impl.expand_hint(this, hint_bytes_to_expand);
	}
}
} // namespace bitscpp
