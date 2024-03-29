#pragma once

#include "cc/bzd/platform/types.hh"
#include "cc/bzd/type_traits/is_floating_point.hh"
#include "cc/bzd/type_traits/is_integral.hh"
#include "cc/bzd/utility/numeric_limits.hh"

#include <cmath>

namespace bzd {

template <concepts::integral T>
constexpr T trunc(const T value) noexcept
{
	return value;
}

template <concepts::floatingPoint T>
constexpr T trunc(const T value) noexcept
{
	// TODO(C++23): use std::trunc directly which is constexpr.
	if (value > static_cast<T>(NumericLimits<Int32>::min()) && value < static_cast<T>(NumericLimits<Int32>::max()))
	{
		return static_cast<Int32>(value);
	}
	if (value > static_cast<T>(NumericLimits<Int64>::min()) && value < static_cast<T>(NumericLimits<Int64>::max()))
	{
		return static_cast<Int64>(value);
	}
	// The difficulty with the implementation is to handle extremely large numbers.
	return std::trunc(value);
}

} // namespace bzd
