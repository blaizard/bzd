#pragma once

namespace bzd { namespace typeTraits {
namespace impl {
template <class T>
struct RemoveConst
{
	typedef T type;
};
template <class T>
struct RemoveConst<const T>
{
	typedef T type;
};
} // namespace impl

template <class T>
using RemoveConst = typename impl::RemoveConst<T>::type;

}} // namespace bzd::typeTraits
