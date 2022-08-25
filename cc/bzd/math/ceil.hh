#pragma once

#include "cc/bzd/math/trunc.hh"

namespace bzd {

template <class T>
constexpr T ceil(const T value) noexcept
{
	const auto t = bzd::trunc(value);
	return t + static_cast<T>((t < value) ? 1 : 0);
}

} // namespace bzd
