#pragma once

namespace bzd::typeTraits::impl {
template <class T>
struct AddVolatile
{
	typedef volatile T type;
};
} // namespace bzd::typeTraits::impl

namespace bzd::typeTraits {
template <class T>
using AddVolatile = typename impl::AddVolatile<T>::type;
} // namespace bzd::typeTraits
