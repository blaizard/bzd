#pragma once

namespace bzd::typeTraits::impl {
template <class T, class... Ts>
struct FirstType
{
	using Type = T;
};

template <class T>
struct FirstType<T>
{
	using Type = T;
};
} // namespace bzd::typeTraits::impl

namespace bzd::typeTraits {
template <class... Ts>
using FirstType = typename impl::FirstType<Ts...>::Type;
} // namespace bzd::typeTraits
