#pragma once

#include "cc/bzd/utility/offset_of.hh"

namespace bzd {
template <class T, class M>
static inline constexpr T* containerOf(M* ptr, const M T::* member)
{
	return reinterpret_cast<T*>(reinterpret_cast<IntPointer>(ptr) - offsetOf(member));
}
} // namespace bzd
