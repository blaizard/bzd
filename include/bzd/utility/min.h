#pragma once

namespace bzd
{
	template <class T>
	constexpr T min(const T& a, const T& b) noexcept
	{
		return (a < b) ? a : b;
	}
}
