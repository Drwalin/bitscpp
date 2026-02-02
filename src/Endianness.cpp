// Copyright (C) 2023-2025 Marek Zalewski aka Drwalin
//
// This file is part of bitscpp project under MIT License
// You should have received a copy of the MIT License along with this program.

#include <cassert>
#include <cstring>
#include <cstdint>

#include <bit>

#include "../include/bitscpp/Endianness.hpp"

namespace bitscpp
{
uint8_t HostToNetworkUint(uint8_t v) { return v; }
uint16_t HostToNetworkUint(uint16_t v)
{
	if constexpr (Endian::little)
		return v;
	else
		return std::byteswap<uint16_t>(v);
}
uint32_t HostToNetworkUint(uint32_t v)
{
	if constexpr (Endian::little)
		return v;
	else
		return std::byteswap<uint32_t>(v);
}
uint64_t HostToNetworkUint(uint64_t v)
{
	if constexpr (Endian::little)
		return v;
	else
		return std::byteswap<uint64_t>(v);
}

uint8_t NetworkToHostUint(uint8_t v) { return HostToNetworkUint(v); }
uint16_t NetworkToHostUint(uint16_t v) { return HostToNetworkUint(v); }
uint32_t NetworkToHostUint(uint32_t v) { return HostToNetworkUint(v); }
uint64_t NetworkToHostUint(uint64_t v) { return HostToNetworkUint(v); }

void WriteBytesInNetworkOrder(uint8_t *buffer, uint64_t value, int bytes)
{
	assert(bytes > 0 && bytes <= 8);
	value = HostToNetworkUint(value);
	memcpy(buffer, &value, bytes);
	// for (int i=0; i<bytes; ++i, ++buffer, value >>= 8) {
	// 	*buffer = value & 0xFF;
	// }
}
void WriteBytesInNetworkOrder(uint8_t *buffer, uint32_t value, int bytes)
{
	assert(bytes > 0 && bytes <= 4);
	value = HostToNetworkUint(value);
	memcpy(buffer, &value, bytes);
	// for (int i=0; i<bytes; ++i, ++buffer, value >>= 8) {
	// 	*buffer = value & 0xFF;
	// }
}
void WriteBytesInNetworkOrder(uint8_t *buffer, uint16_t value, int bytes)
{
	assert(bytes > 0 && bytes <= 2);
	value = HostToNetworkUint(value);
	memcpy(buffer, &value, bytes);
	// for (int i=0; i<bytes; ++i, ++buffer, value >>= 8) {
	// 	*buffer = value & 0xFF;
	// }
}
uint64_t ReadBytesInNetworkOrder(const uint8_t *buffer, int bytes)
{
	assert(bytes > 0 && bytes <= 8);
	uint64_t value = 0;
	memcpy(&value, buffer, bytes);
	// for (int i=0; i<bytes; ++i, ++buffer, value >>= 8) {
	// 	*buffer = value & 0xFF;
	// }
	return NetworkToHostUint(value);
}
} // namespace bitscpp
