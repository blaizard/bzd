#pragma once

#include "cc/bzd/platform/types.hh"

namespace bzd {

namespace concepts {
template <class T>
concept begin = requires(T t)
{
	t.begin();
};
} // namespace concepts

template <class T>
requires concepts::begin<T>
[[nodiscard]] constexpr auto begin(T& t)
{
	return t.begin();
}

template <class T, Size n>
[[nodiscard]] constexpr T* begin(T (&arr)[n])
{
	return arr;
}

} // namespace bzd
