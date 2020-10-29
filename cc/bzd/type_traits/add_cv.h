#pragma once

namespace bzd::typeTraits::impl {
template <class T>
struct AddCV
{
	typedef const volatile T type;
};
} // namespace bzd::typeTraits::impl

namespace bzd::typeTraits {
template <class T>
using AddCV = typename impl::AddCV<T>::type;
} // namespace bzd::typeTraits
