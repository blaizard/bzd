#pragma once

#include "cc/bzd/platform/types.hh"

namespace bzd {

namespace concepts {
template <class T>
concept size = requires(T& t)
{
	t.size();
};
} // namespace concepts

template <class T>
requires concepts::size<T>
[[nodiscard]] constexpr bzd::Size size(T& t) { return t.size(); }

template <class T, Size n>
[[nodiscard]] constexpr bzd::Size size(T (&)[n])
{
	return n;
}

} // namespace bzd
