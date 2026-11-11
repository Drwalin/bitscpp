// Copyright (C) 2023-2025 Marek Zalewski aka Drwalin
//
// This file is part of bitscpp project under MIT License
// You should have received a copy of the MIT License along with this program.

#ifndef BITSCPP_BYTE_READER_V1_EXTENSIONS_HPP
#define BITSCPP_BYTE_READER_V1_EXTENSIONS_HPP

#include <set>
#include <unordered_set>

#include "ByteReader.hpp"

namespace bitscpp
{
namespace deprecated
{
template <typename T>
inline ByteReader<true> &op(ByteReader<true> &s, std::set<T> &set)
{
	uint32_t elements;
	s.op(elements);
	for (uint32_t i = 0; i < elements; ++i) {
		T v;
		s.op(v);
		set.emplace_hint(set.end(), std::move(v));
	}
	return s;
}
template <typename T>
inline ByteReader<true> &op(ByteReader<true> &s, std::unordered_set<T> &set)
{
	uint32_t elements;
	s.op(elements);
	for (uint32_t i = 0; i < elements; ++i) {
		T v;
		s.op(v);
		set.insert(std::move(v));
	}
	return s;
}

template <typename T>
inline ByteReader<false> &op(ByteReader<false> &s, std::set<T> &set)
{
	uint32_t elements;
	s.op(elements);
	for (uint32_t i = 0; i < elements; ++i) {
		T v;
		s.op(v);
		set.emplace_hint(set.end(), std::move(v));
	}
	return s;
}
template <typename T>
inline ByteReader<false> &op(ByteReader<false> &s, std::unordered_set<T> &set)
{
	uint32_t elements;
	s.op(elements);
	for (uint32_t i = 0; i < elements; ++i) {
		T v;
		s.op(v);
		set.insert(std::move(v));
	}
	return s;
}
} // namespace deprecated
} // namespace bitscpp

#endif
