/*
 *  This file is part of bitscpp.
 *  Copyright (C) 2023 Marek Zalewski aka Drwalin
 *
 *  bitscpp is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  bitscpp is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef BITSCPP_ENDIANNESS_HPP
#define BITSCPP_ENDIANNESS_HPP

#include <cinttypes>
#include <cstdint>

namespace bitscpp {

	// https://stackoverflow.com/questions/1583791/constexpr-and-endianness
	// answer by Piotr Siupa
	class Endian
	{
	private:
		static constexpr uint32_t uint32_ = 0x01020304;
		static constexpr uint8_t magic_ = (const uint8_t&)uint32_;
	public:
		static constexpr bool little = magic_ == 0x04;
		static constexpr bool big = magic_ == 0x01;
		static_assert(little || big, "Cannot determine endianness!");
	private:
		Endian() = delete;
	};
	
	constexpr bool IsBigEndian() {
		return Endian::big;
	}
	
	template<typename T>
	inline T HostToNetworkUint(T v) {
		if constexpr (!IsBigEndian()) {
			return v;
		} else {
			if constexpr (sizeof(T) == 1) {
				return v;
			} else if constexpr (sizeof(T) == 2) {
				return (v>>8) | (v<<8);
			} else if constexpr (sizeof(T) == 4) {
				uint16_t a, b;
				a = v;
				b = v>>16;
				return (((T)HostToNetworkUint(a))<<16) | (T)HostToNetworkUint(b);
			} else if constexpr (sizeof(T) == 8) {
				uint32_t a, b;
				a = v;
				b = v>>32;
				return (((T)HostToNetworkUint(a))<<32) | (T)HostToNetworkUint(b);
			}
		}
	}
}

#endif

