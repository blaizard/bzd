#pragma once

#include "cc/bzd/type_traits/is_reference.hh"
#include "cc/bzd/type_traits/is_void.hh"

namespace bzd::typeTraits::impl {
template <typename T, bool b>
struct AddRValueReferenceHelper
{
	typedef T type;
};

template <typename T>
struct AddRValueReferenceHelper<T, true>
{
	typedef T&& type;
};

template <typename T>
struct AddRValueReference
{
	typedef typename AddRValueReferenceHelper<T, (isVoid<T> == false && isReference<T> == false)>::type type;
};
} // namespace bzd::typeTraits::impl

namespace bzd::typeTraits {
template <class T>
using AddRValueReference = typename impl::AddRValueReference<T>::type;

} // namespace bzd::typeTraits
