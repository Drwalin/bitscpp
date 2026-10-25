// Copyright (C) 2026 Marek Zalewski aka Drwalin
//
// This file is part of bitscpp project under MIT License
// You should have received a copy of the MIT License along with this program.

#ifndef BITSCPP_FORWARD_HPP
#define BITSCPP_FORWARD_HPP

#include "GenericBufferWriterPimpl.hpp" // IWYU pragma: keep

namespace bitscpp
{
namespace v2_2
{
class ByteWriter;
class ByteReader;
}
class GenericBufferWriter;
}

#if not defined BITSCPP_INLINE
# if BITSCPP_ENABLE_INLINE_V2
#  define BITSCPP_INLINE inline
#  warning INLINE ---------
# else
#  define BITSCPP_INLINE
#  warning NO INLINE ---------
# endif
#endif

#if not defined BITSCPP_INLINE_BOOL
# if BITSCPP_ENABLE_INLINE_V2
#  define BITSCPP_INLINE_BOOL
# endif
#endif

#endif
