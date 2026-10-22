// Copyright (C) 2026 Marek Zalewski aka Drwalin
//
// This file is part of bitscpp project under MIT License
// You should have received a copy of the MIT License along with this program.

#ifndef BITSCPP_GENERIC_BUFFER_WRAPPER_HPP
#define BITSCPP_GENERIC_BUFFER_WRAPPER_HPP

#include <cstdint>
#include <cstdlib>

namespace bitscpp
{
struct GenericBufferWriter;

/*
 * Example usage 1:
 * 
 * UnderlyingBuffer under_buf{...};
 * static GenericBufferWriterImpl impl{...};
 * GenericBufferWriter buffer(&under_buf, impl);
 * v3::ByteWriter writer(std::move(buffer));
 * // do writes here
 * buffer = writer.release_buffer();
 * ((ignore_ret_value)) buffer.release_buffer();
 * 
 * now under_buf is filled and ready to use
 */

/*
 * Example usage 2:
 * 
 * static GenericBufferWriterImpl impl{...};
 * GenericBufferWriter buffer(impl);
 * v3::ByteWriter writer(std::move(buffer));
 * // do writes here
 * buffer = writer.release_buffer();
 * UnderlyingBuffer *under_buf = buffer.release_buffer();
 * 
 * now *under_buf is filled and ready to use
 */

struct GenericBufferWriterImpl {
	// Construct underlying buffer if it is not present.
	// Guarantees to return at least 1 byte available in head..end range.
	// Best effort to reserve around hint_needed_bytes bytes.
	// set head and end
	void (*expand_hint)(GenericBufferWriter *buffer, size_t hint_needed_bytes) = nullptr;

	// Construct underlying buffer if it is not present.
	// Guarantees range head..end points to range of at least needed_bytes bytes
	// available.
	// Sets head and end pointers.
	// Due to performance reasons and possible implementation optimizations,
	// maximum of smallest guaranteed available range is 255 
	void (*expand)(GenericBufferWriter *buffer, uint8_t needed_bytes) = nullptr;

	// destructs and deallocates underlying buffer - caller needs to take care
	// of wether object is allocated on stack or heap.
	void (*destructor)(GenericBufferWriter *buffer) = nullptr;

	// Releases underlying buffer from GenericBufferWriter. After call
	// underlying buffer needs to be returned, and:
	// buffer, head and end need to be set to nullptr by GenericBufferWriter.
	void (*release_underlying_buffer)(GenericBufferWriter *buffer) = nullptr;
};

struct GenericBufferWriter {
	/*
	 * Takes ownership of buffer. If buffer is allocated on the stack, then
	 * caller needs to guarantee that impl.destructor does not deallocates
	 * *buffer.
	 */
	GenericBufferWriter(void *buffer, GenericBufferWriterImpl impl);
	
	GenericBufferWriter(GenericBufferWriterImpl impl);
	GenericBufferWriter(const GenericBufferWriter&) = delete;
	GenericBufferWriter(GenericBufferWriter&) = delete;
	GenericBufferWriter(GenericBufferWriter&&);
	GenericBufferWriter &operator=(const GenericBufferWriter&) = delete;
	GenericBufferWriter &operator=(GenericBufferWriter&) = delete;
	GenericBufferWriter &operator=(GenericBufferWriter&&);
	// calls impl.destructor
	~GenericBufferWriter();

	// assimilates head+end into underlying buffer
	void *release_buffer();

	void append_byte(const uint8_t byte) noexcept;
	void append(const uint8_t *data, size_t bytes) noexcept;

	/*
	 * Returns pointer to empty needed_bytes. Returned range is considered
	 * already written to buffer - allows for zero-copy appending
	 */
	uint8_t *expand(uint8_t needed_bytes) noexcept;

	/*
	 * Calles impl.expand_hint
	 */
	void expand_hint(size_t hint_needed_bytes) noexcept;

private:
	void append_slow(const uint8_t *data, size_t bytes) noexcept;

public:
	GenericBufferWriterImpl impl;
	// pointer to underlying buffer
	void *buffer = nullptr;
	// pointer to the begining of available range, increment to mark data used.
	uint8_t *head = nullptr;
	// pointer to the end of available range.
	uint8_t *end = nullptr;
};
} // namespace bitscpp

#endif
