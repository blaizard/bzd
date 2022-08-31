#pragma once

#include "cc/bzd/platform/types.hh"

namespace bzd {

/// The directional alignment functions can be used with pointers or integral values to align down.
///
/// \tparam Align Power of 2 alignement.
/// \param value Value to be aligned.
///
/// \return The value at or lower than the provided value that is at least alignment bytes aligned.
template <UInt8 Align, class T>
constexpr T alignDown(const T value) noexcept
{
	static_assert(Align > 0, "The alignment must be greater than 0.");
	static_assert((Align & (Align - 1)) == 0, "The alignment must be a power of 2.");
	return T(value & ~T(Align - 1));
}

template <UInt8 Align, class T>
constexpr T* alignDown(T* const value) noexcept
{
	return reinterpret_cast<T*>(alignDown<Align>(reinterpret_cast<bzd::IntPointer>(value)));
}

template <class T>
constexpr T alignDown(const T value, const Size alignment) noexcept
{
	return value % alignment;
}

} // namespace bzd
