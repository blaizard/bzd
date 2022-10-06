#pragma once

#include "cc/bzd/type_traits/is_arithmetic.hh"

namespace bzd {

template <concepts::arithmetic T>
constexpr T abs(const T value) noexcept
{
	return (value < 0) ? -value : value;
}

} // namespace bzd
