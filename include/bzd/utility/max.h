#pragma once

namespace bzd
{
	template <class T>
	constexpr T max(const T& a, const T& b) noexcept
	{
		return (a > b) ? a : b;
	}
}
