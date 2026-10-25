// Copyright (C) 2026 Marek Zalewski aka Drwalin
//
// This file is part of bitscpp project under MIT License
// You should have received a copy of the MIT License along with this program.

#ifndef BITSCPP_GENERIC_BUFFER_WRAPPER_PIMPL_HPP
#define BITSCPP_GENERIC_BUFFER_WRAPPER_PIMPL_HPP

#include <cstdint>

namespace bitscpp
{
class GenericBufferWriter;

struct GenericBufferWriterPimpl {
	// Construct underlying buffer if it is not present.
	// Guarantees range head..end points to range of at least minimum bytes
	// available, up to a hint.
	// Sets head and end pointers.
	// Due to performance reasons and possible implementation optimizations,
	// maximum of smallest guaranteed available range is 255
	void (*expand)(GenericBufferWriter *buffer, uint8_t minimum,
				   std::size_t hint) = nullptr;

	// destructs and deallocates underlying buffer - caller needs to take care
	// of wether object is allocated on stack or heap.
	void (*destructor)(GenericBufferWriter *buffer) = nullptr;

	// Releases underlying buffer from GenericBufferWriter. After call
	// underlying buffer needs to be returned, and:
	// buffer, head and end need to be set to nullptr by GenericBufferWriter.
	void (*apply_head_end_to_underlying)(GenericBufferWriter *buffer) = nullptr;
};
} // namespace bitscpp

#endif
