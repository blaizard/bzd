#pragma once

#include "cc/bzd/platform/types.hh"

namespace bzd {

namespace concepts {
template <class T>
concept end = requires(T t)
{
	t.end();
};
} // namespace concepts

template <class T>
requires concepts::end<T>
constexpr auto end(T& t)
{
	return t.end();
}

template <class T, SizeType n>
constexpr T* end(T (&arr)[n])
{
	return arr + n;
}

} // namespace bzd
