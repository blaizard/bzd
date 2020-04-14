#pragma once

namespace bzd { namespace typeTraits {
namespace impl {
template <class T>
struct RemoveReference
{
	typedef T type;
};

template <class T>
struct RemoveReference<T&>
{
	typedef T type;
};

template <class T>
struct RemoveReference<T&&>
{
	typedef T type;
};
} // namespace impl

template <class T>
using RemoveReference = typename impl::RemoveReference<T>::type;

}} // namespace bzd::typeTraits
