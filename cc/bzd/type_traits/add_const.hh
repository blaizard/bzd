#pragma once

namespace bzd::typeTraits::impl {
template <class T>
struct AddConst
{
	typedef const T type;
};
} // namespace bzd::typeTraits::impl

namespace bzd::typeTraits {
template <class T>
using AddConst = typename impl::AddConst<T>::type;

} // namespace bzd::typeTraits
