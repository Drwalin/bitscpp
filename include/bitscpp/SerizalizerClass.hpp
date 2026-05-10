// Copyright (C) 2023-2026 Marek Zalewski aka Drwalin
//
// This file is part of bitscpp project under MIT License
// You should have received a copy of the MIT License along with this program.

#ifndef BITSCPP_SERIALIZER_CLASS_HPP
#define BITSCPP_SERIALIZER_CLASS_HPP

namespace bitscpp
{
template <typename S, typename T> struct serializer;
} // namespace bitscpp

#define BITSCPP_DEFINE_INLINE_SERIALIZE_METHOD(SERIALIZER, CODE) \
	inline void serialize(auto &SERIALIZER) { CODE } \
	inline void serialize(auto &SERIALIZER) const { CODE }

#endif
