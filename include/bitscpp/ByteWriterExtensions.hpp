// Copyright (C) 2023-2025 Marek Zalewski aka Drwalin
// 
// This file is part of bitscpp project under MIT License
// You should have received a copy of the MIT License along with this program.

#ifndef BITSCPP_BYTE_WRITER_EXTENSIONS_HPP
#define BITSCPP_BYTE_WRITER_EXTENSIONS_HPP

#include <set>
#include <unordered_set>

#include "ByteWriter.hpp"

namespace bitscpp {
	template<typename BT, typename T>
	inline ByteWriter<BT>& op(ByteWriter<BT>& s, const std::set<T>& set) {
		s.op((uint32_t)set.size());
		for(auto& v : set)
			s.op(v);
		return s;
	}
	
	template<typename BT, typename T>
	inline ByteWriter<BT>& op(ByteWriter<BT>& s, const std::unordered_set<T>& set) {
		s.op((uint32_t)set.size());
		for(auto& v : set)
			s.op(v);
		return s;
	}
} // namespace bitscpp

#endif

