#pragma once

#include "cc/bzd/platform/types.hh"

namespace bzd {
template <class T, class M>
static inline constexpr IntPointer offsetOf(const M T::* member)
{
	return reinterpret_cast<IntPointer>(&(reinterpret_cast<T*>(0)->*member));
}
} // namespace bzd
