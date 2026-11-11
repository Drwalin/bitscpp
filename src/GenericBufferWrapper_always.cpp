// Copyright (C) 2026 Marek Zalewski aka Drwalin
//
// This file is part of bitscpp project under MIT License
// You should have received a copy of the MIT License along with this program.

#include <cassert>
#include <cstring>

#include "../include/bitscpp/GenericBufferWrapper.hpp"

namespace bitscpp
{
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
} // namespace bitscpp
