#pragma once

#include "bzd/types.h"

namespace bzd {
template <class T, class M>
static inline constexpr IntPtrType offsetOf(const M T::*member)
{
	return reinterpret_cast<IntPtrType>(&(reinterpret_cast<T*>(0)->*member));
}
} // namespace bzd
