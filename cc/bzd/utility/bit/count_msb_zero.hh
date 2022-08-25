#pragma once

#include "cc/bzd/platform/types.hh"

#include <bit>

namespace bzd {

/// Returns the number of consecutive 0 bits, starting from the most significant bit.
///
/// \param value The value to be used.
/// \return The number of consecutive 0 bits.
template <class T>
constexpr Size countMSBZero(const T value) noexcept
{
	return ::std::countl_zero(value);
}

} // namespace bzd
