#pragma once

namespace bzd { namespace typeTraits {
namespace impl {
template <bool Condition, class T = void>
struct EnableIf
{
};

template <class T>
struct EnableIf<true, T>
{
	typedef T type;
};
} // namespace impl

template <bool Condition, class T = void>
using EnableIf = typename impl::EnableIf<Condition, T>::type;
}} // namespace bzd::typeTraits
