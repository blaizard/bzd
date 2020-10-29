#pragma once

namespace bzd::typeTraits::impl {
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
} // namespace bzd::typeTraits::impl

namespace bzd::typeTraits {
template <class T>
using RemoveReference = typename impl::RemoveReference<T>::type;

} // namespace bzd::typeTraits
