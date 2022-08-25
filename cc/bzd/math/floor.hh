#pragma once

#include "cc/bzd/math/trunc.hh"

namespace bzd {

template <class T>
constexpr T floor(const T value) noexcept
{
	const auto t = bzd::trunc(value);
	if (value >= 0 || value == t)
	{
		return t;
	}
	return t - 1;
}

} // namespace bzd
