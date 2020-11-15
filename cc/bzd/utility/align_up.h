#pragma once

#include "bzd/platform/types.h"

namespace bzd {
/**
 * The directional alignment functions can be used with pointers or integral values to align up.
 *
 * \tparam Align Power of 2 alignement.
 * \param value Value to be aligned.
 *
 * \return The value at or after than the provided value that is at least alignment bytes aligned.
 */
template <UInt8Type Align, class T>
constexpr T alignUp(T value) noexcept
{
	static_assert(Align > 0, "The alignment must be greater than 0.");
	static_assert((Align & (Align - 1)) == 0, "The alignment must be a power of 2.");
	return T((value + (T(Align) - 1)) & ~T(Align - 1));
}
template <UInt8Type Align, class T>
constexpr T* alignUp(T* value) noexcept
{
	return reinterpret_cast<T*>(alignUp<Align>(reinterpret_cast<bzd::IntPtrType>(value)));
}
} // namespace bzd
