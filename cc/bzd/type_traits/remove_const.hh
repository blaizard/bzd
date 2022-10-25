#pragma once

namespace bzd::typeTraits::impl {
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
template <class T>
struct RemoveConst<const T&>
{
	typedef T& type;
};
} // namespace bzd::typeTraits::impl

namespace bzd::typeTraits {
template <class T>
using RemoveConst = typename impl::RemoveConst<T>::type;

} // namespace bzd::typeTraits
