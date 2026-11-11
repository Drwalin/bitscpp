// Copyright (C) 2023-2025 Marek Zalewski aka Drwalin
// 
// This file is part of bitscpp project under MIT License
// You should have received a copy of the MIT License along with this program.

#ifndef BITSCPP_BYTE_WRITER_V2_EXTENSIONS_HPP
#define BITSCPP_BYTE_WRITER_V2_EXTENSIONS_HPP

#include <set>
#include <unordered_set>

#include "ByteWriter_v2.hpp" // IWYU pragma: export

namespace bitscpp {
template<typename BT, typename T>
struct serializer<v2::deprecated::ByteWriter<BT>, std::set<T>> {
	static inline void op(v2::deprecated::ByteWriter<BT>& s, const std::set<T>& set) {
		assert(set.size() <= v2::MAX_ARRAY_ELEMENTS);
		if (set.size() > v2::MAX_ARRAY_ELEMENTS) {
			s.set_error(v2::ERROR_ARRAY_TOO_BIG);
			return;
		}
		s.op((uint32_t)set.size());
		for(auto& v : set)
			s.op(v);
	}
};
template<typename BT, typename T>
struct serializer<v2::deprecated::ByteWriter<BT>, std::unordered_set<T>> {
	static inline void op(v2::deprecated::ByteWriter<BT>& s, const std::unordered_set<T>& set) {
		assert(set.size() <= v2::MAX_ARRAY_ELEMENTS);
		if (set.size() > v2::MAX_ARRAY_ELEMENTS) {
			s.set_error(v2::ERROR_ARRAY_TOO_BIG);
			return;
		}
		s.op((uint32_t)set.size());
		for(auto& v : set)
			s.op(v);
	}
};
} // namespace bitscpp

#endif

