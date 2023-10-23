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

#ifndef BITSCPP_BYTE_WRITER_EXTENSIONS_HPP
#define BITSCPP_BYTE_WRITER_EXTENSIONS_HPP

#include <set>
#include <unordered_set>
#include <tuple>

#include "ByteWriter.hpp"

namespace bitscpp {
	template<typename T, typename... Args>
	inline ByteWriter& op(ByteWriter& s, const std::set<T>& set, Args... args) {
		s.op((uint32_t)set.size());
		for(auto& v : set)
			s.op(v, args...);
		return s;
	}
	
	template<typename T, typename... Args>
	inline ByteWriter& op(ByteWriter& s, const std::unordered_set<T>& set, Args... args) {
		s.op((uint32_t)set.size());
		for(auto& v : set)
			s.op(v, args...);
		return s;
	}
} // namespace bitscpp

#endif

