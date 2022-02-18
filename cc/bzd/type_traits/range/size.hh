#pragma once

#include "cc/bzd/platform/types.hh"

namespace bzd {

namespace concepts {
template <class T>
concept size = requires(T t)
{
	t.size();
};
} // namespace concepts

template <class T>
requires concepts::size<T>
[[nodiscard]] constexpr auto size(T& t)
{
	return t.size();
}

template <class T, SizeType n>
[[nodiscard]] constexpr bzd::SizeType size(T (&)[n])
{
	return n;
}

} // namespace bzd
