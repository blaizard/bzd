#pragma once

#include "cc/bzd/platform/types.hh"

#include <bit>

namespace bzd {

/// Returns the number of consecutive 1 bits, starting from the least significant bit.
///
/// \param value The value to be used.
/// \return The number of consecutive 1 bits.
template <class T>
constexpr Size countLSBOne(const T value) noexcept
{
	return ::std::countr_one(value);
}

} // namespace bzd
