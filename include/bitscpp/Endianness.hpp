// Copyright (C) 2023-2025 Marek Zalewski aka Drwalin
// 
// This file is part of bitscpp project under MIT License
// You should have received a copy of the MIT License along with this program.

#ifndef BITSCPP_ENDIANNESS_HPP
#define BITSCPP_ENDIANNESS_HPP

#include <cstdint>

#include <bit>

namespace bitscpp {
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
	
	inline uint8_t HostToNetworkUint(uint8_t v) {
		return v;
	}
	
	inline uint16_t HostToNetworkUint(uint16_t v) {
		if constexpr (Endian::little) {
			return v;
		} else {
			return std::byteswap<uint16_t>(v);
		}
	}
	
	inline uint32_t HostToNetworkUint(uint32_t v) {
		if constexpr (Endian::little) {
			return v;
		} else {
			return std::byteswap<uint32_t>(v);
		}
	}
	
	inline uint64_t HostToNetworkUint(uint64_t v) {
		if constexpr (Endian::little) {
			return v;
		} else {
			return std::byteswap<uint64_t>(v);
		}
	}
}

#endif

