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

#ifndef BITSCPP_BYTE_READER_EXTENSIONS_HPP
#define BITSCPP_BYTE_READER_EXTENSIONS_HPP

#include <set>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <functional>

#include "ByteReader.hpp"

namespace bitscpp {
	template<typename T, typename... Args>
	inline ByteReader& op(ByteReader& s, std::set<T>& set, Args... args) {
		uint32_t bytes;
		s.op(bytes);
		for(uint32_t i=0; i<bytes; ++i) {
			T v;
			s.op(v, args...);
			set.emplace_hint(set.end(), std::move(v));
		}
		return s;
	}
	
	template<typename T, typename... Args>
	inline ByteReader& op(ByteReader& s, std::unordered_set<T>& set, Args... args) {
		uint32_t bytes;
		s.op(bytes);
		for(uint32_t i=0; i<bytes; ++i) {
			T v;
			s.op(v, args...);
			set.insert(std::move(v));
		}
		return s;
	}
} // namespace bitscpp

#endif

