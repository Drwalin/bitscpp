// Copyright (C) 2023-2025 Marek Zalewski aka Drwalin
//
// This file is part of bitscpp project under MIT License
// You should have received a copy of the MIT License along with this program.

#pragma once
#ifndef BITSCPP_ENDIANNESS_INL_HPP
#define BITSCPP_ENDIANNESS_INL_HPP

#include <cassert>
#include <cstdint>

#include <bit>

#include "../include/bitscpp/Endianness.hpp"

namespace bitscpp
{
inline uint8_t HostToNetworkUint(uint8_t v) { return v; }
inline uint16_t HostToNetworkUint(uint16_t v)
{
	if constexpr (Endian::little)
		return v;
	else
		return std::byteswap<uint16_t>(v);
}
inline uint32_t HostToNetworkUint(uint32_t v)
{
	if constexpr (Endian::little)
		return v;
	else
		return std::byteswap<uint32_t>(v);
}
inline uint64_t HostToNetworkUint(uint64_t v)
{
	if constexpr (Endian::little)
		return v;
	else
		return std::byteswap<uint64_t>(v);
}

inline uint8_t NetworkToHostUint(uint8_t v) { return HostToNetworkUint(v); }
inline uint16_t NetworkToHostUint(uint16_t v) { return HostToNetworkUint(v); }
inline uint32_t NetworkToHostUint(uint32_t v) { return HostToNetworkUint(v); }
inline uint64_t NetworkToHostUint(uint64_t v) { return HostToNetworkUint(v); }

inline void WriteBytesInNetworkOrder(uint8_t *buffer, uint64_t value, int bytes)
{
	assert(bytes > 0 && bytes <= 8);
	uint8_t *end = buffer + bytes;
	for (; buffer != end; ++buffer, value >>= 8) {
		*buffer = value; 
	}
}
inline void WriteBytesInNetworkOrder(uint8_t *buffer, uint32_t value, int bytes)
{
	assert(bytes > 0 && bytes <= 4);
	uint8_t *end = buffer + bytes;
	for (; buffer != end; ++buffer, value >>= 8) {
		*buffer = value; 
	}
}
inline void WriteBytesInNetworkOrder(uint8_t *buffer, uint16_t value, int bytes)
{
	assert(bytes > 0 && bytes <= 2);
	uint8_t *end = buffer + bytes;
	for (; buffer != end; ++buffer, value >>= 8) {
		*buffer = value;
	}
}
inline uint64_t ReadBytesInNetworkOrder(uint8_t const *buffer, int bytes)
{
	assert(bytes > 0 && bytes <= 8);

	/*
	uint64_t const *a = (uint64_t const *)(((intptr_t)buffer) & (~7lu));
	uint64_t va = NetworkToHostUint(*a);

	uint64_t const *b = (uint64_t const *)(((intptr_t)buffer + bytes - 1) & (~7lu));

	assert((intptr_t)a <= (intptr_t)buffer);
	assert((intptr_t)b >= (intptr_t)a);
	
	uint64_t v;

	if (a == b) {
		uint32_t of1 = buffer - (uint8_t const *)a;
		uint32_t off1 = of1 << 3;
		assert(off1 >= 0);
		assert(off1 < 64);

		v = (va >> off1);
	} else {
		uint64_t vb = NetworkToHostUint(*b);
		uint32_t of1 = buffer - (uint8_t const *)a;
		uint32_t of2 = (uint8_t const *)b - buffer;
		uint32_t off1 = of1 << 3;
		uint32_t off2 = of2 << 3;
		assert(off1 >= 0);
		assert(off1 < 64);
		assert(off2 >= 0);
		assert(off2 < 64);

		v = (va >> off1) | (vb << off2);
	}


	v &= (~0lu) >> (64 - (bytes << 3));

	return NetworkToHostUint(v);
	*/

	uint64_t v = 0;
	switch(bytes) {
	case 8:
		v |= ((uint64_t)buffer[7]) << 56;
	case 7:
		v |= ((uint64_t)buffer[6]) << 48;
	case 6:
		v |= ((uint64_t)buffer[5]) << 40;
	case 5:
		v |= ((uint64_t)buffer[4]) << 32;
	case 4:
		v |= ((uint64_t)buffer[3]) << 24;
	case 3:
		v |= ((uint64_t)buffer[2]) << 16;
	case 2:
		v |= ((uint64_t)buffer[1]) << 8;
	case 1:
		v |= ((uint64_t)buffer[0]);
	}
	return v;



	uint64_t value = 0;
	uint8_t const *end = buffer - 1;
	buffer = end + bytes;
	for (; buffer != end; --buffer) {
		value <<= 8;
		value |= *buffer;
	}
	return value;
}
} // namespace bitscpp

#endif
