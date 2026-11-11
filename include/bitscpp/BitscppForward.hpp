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
}
using namespace v2_2;

namespace v2
{
class ByteReader;
}
using namespace v2;

class GenericBufferWriter;
}

#if not defined _BITSCPP_INLINE
# if BITSCPP_ENABLE_INLINE_V2
#  define _BITSCPP_INLINE inline
# else
#  define _BITSCPP_INLINE
# endif
#endif

#if not defined BITSCPP_INLINE_BOOL
# if BITSCPP_ENABLE_INLINE_V2
#  define BITSCPP_INLINE_BOOL
# endif
#endif

#endif
