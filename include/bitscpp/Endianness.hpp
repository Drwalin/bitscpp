// Copyright (C) 2023-2025 Marek Zalewski aka Drwalin
//
// This file is part of bitscpp project under MIT License
// You should have received a copy of the MIT License along with this program.

#ifndef BITSCPP_ENDIANNESS_HPP
#define BITSCPP_ENDIANNESS_HPP

#include <cstdint>

#include <bit>

namespace bitscpp
{
class Endian
{
public:
	static constexpr bool little = std::endian::native == std::endian::little;
	static constexpr bool big = std::endian::native == std::endian::big;
	static_assert(little || big, "Cannot determine endianness!");
	static constexpr bool IsLittle() { return little; }
	static constexpr bool IsBig() { return big; }

private:
	Endian() = delete;
};

uint8_t HostToNetworkUint(uint8_t v);
uint16_t HostToNetworkUint(uint16_t v);
uint32_t HostToNetworkUint(uint32_t v);
uint64_t HostToNetworkUint(uint64_t v);

uint8_t NetworkToHostUint(uint8_t v);
uint16_t NetworkToHostUint(uint16_t v);
uint32_t NetworkToHostUint(uint32_t v);
uint64_t NetworkToHostUint(uint64_t v);

void WriteBytesInNetworkOrder(uint8_t *buffer, uint64_t value, int bytes);
uint64_t ReadBytesInNetworkOrder(uint8_t *buffer, int bytes);
} // namespace bitscpp

#endif
