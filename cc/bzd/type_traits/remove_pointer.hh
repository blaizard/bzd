#pragma once

namespace bzd::typeTraits::impl {
template <class T>
struct RemovePointer
{
	typedef T type;
};
template <class T>
struct RemovePointer<T*>
{
	typedef T type;
};
} // namespace bzd::typeTraits::impl

namespace bzd::typeTraits {
template <class T>
using RemovePointer = typename impl::RemovePointer<T>::type;

} // namespace bzd::typeTraits
