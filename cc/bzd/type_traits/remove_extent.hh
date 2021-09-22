#pragma once

namespace bzd::typeTraits::impl {
template <class T>
struct RemoveExtent
{
	typedef T type;
};
template <class T>
struct RemoveExtent<T[]>
{
	typedef T type;
};
template <class T, unsigned long int N>
struct RemoveExtent<T[N]>
{
	typedef T type;
};
} // namespace bzd::typeTraits::impl

namespace bzd::typeTraits {
template <class T>
using RemoveExtent = typename impl::RemoveExtent<T>::type;

} // namespace bzd::typeTraits
