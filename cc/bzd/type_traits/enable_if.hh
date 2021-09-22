#pragma once

namespace bzd::typeTraits::impl {
template <bool Condition, class T = void>
struct EnableIf
{
};

template <class T>
struct EnableIf<true, T>
{
	typedef T type;
};
} // namespace bzd::typeTraits::impl

namespace bzd::typeTraits {
template <bool Condition, class T = void>
using EnableIf = typename impl::EnableIf<Condition, T>::type;
} // namespace bzd::typeTraits
