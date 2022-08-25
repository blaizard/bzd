#pragma once

#include "cc/bzd/platform/types.hh"

#include <bit>

namespace bzd {

/// Returns the number of consecutive 0 bits, starting from the least significant bit.
///
/// \param value The value to be used.
/// \return The number of consecutive 0 bits.
template <class T>
constexpr Size countLSBZero(const T value) noexcept
{
	return ::std::countr_zero(value);
}

} // namespace bzd
