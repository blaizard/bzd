#pragma once

namespace bzd {
template <class T>
constexpr T min(const T& a) noexcept
{
	return a;
}

template <class T, class... Ts>
constexpr T min(const T& a, const T& b, const Ts&... n) noexcept
{
	return bzd::min((a < b) ? a : b, n...);
}
} // namespace bzd
