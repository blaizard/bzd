#pragma once

namespace bzd {
template <class T>
constexpr T max(const T &a) noexcept
{
	return a;
}

template <class T, class... Ts>
constexpr T max(const T &a, const T &b, const Ts &... n) noexcept
{
	return bzd::max((a > b) ? a : b, n...);
}
} // namespace bzd
