// Copyright (C) 2023-2025 Marek Zalewski aka Drwalin
// 
// This file is part of bitscpp project under MIT License
// You should have received a copy of the MIT License along with this program.

#ifndef BITSCPP_BYTE_READER_EXTENSIONS_HPP
#define BITSCPP_BYTE_READER_EXTENSIONS_HPP

#include <set>
#include <unordered_set>

#include "ByteReader.hpp"
#include "ByteReader_v2.hpp"

namespace bitscpp {
template<typename T>
inline ByteReader<true>& op(ByteReader<true>& s, std::set<T>& set) {
	uint32_t elements;
	s.op(elements);
	for(uint32_t i=0; i<elements; ++i) {
		T v;
		s.op(v);
		set.emplace_hint(set.end(), std::move(v));
	}
	return s;
}
template<typename T>
inline ByteReader<true>& op(ByteReader<true>& s, std::unordered_set<T>& set) {
	uint32_t elements;
	s.op(elements);
	for(uint32_t i=0; i<elements; ++i) {
		T v;
		s.op(v);
		set.insert(std::move(v));
	}
	return s;
}

template<typename T>
inline ByteReader<false>& op(ByteReader<false>& s, std::set<T>& set) {
	uint32_t elements;
	s.op(elements);
	for(uint32_t i=0; i<elements; ++i) {
		T v;
		s.op(v);
		set.emplace_hint(set.end(), std::move(v));
	}
	return s;
}
template<typename T>
inline ByteReader<false>& op(ByteReader<false>& s, std::unordered_set<T>& set) {
	uint32_t elements;
	s.op(elements);
	for(uint32_t i=0; i<elements; ++i) {
		T v;
		s.op(v);
		set.insert(std::move(v));
	}
	return s;
}

template<typename T>
struct serializer<v2::ByteReader, std::set<T>> {
static inline void op(v2::ByteReader& s, std::set<T>& set) {
	uint32_t elements;
	s.op(elements);
	for(uint32_t i=0; i<elements; ++i) {
		T v;
		s.op(v);
		set.emplace_hint(set.end(), std::move(v));
	}
}
};
template<typename T>
struct serializer<v2::ByteReader, std::unordered_set<T>> {
static inline v2::ByteReader& op(v2::ByteReader& s, std::unordered_set<T>& set) {
	uint32_t elements;
	s.op(elements);
	for(uint32_t i=0; i<elements; ++i) {
		T v;
		s.op(v);
		set.insert(std::move(v));
	}
	return s;
}
};
} // namespace bitscpp

#endif

