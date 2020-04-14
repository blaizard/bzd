#pragma once

#include "bzd/type_traits/is_reference.h"
#include "bzd/type_traits/is_void.h"

namespace bzd { namespace typeTraits {
namespace impl {
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
} // namespace impl

template <class T>
using AddRValueReference = typename impl::AddRValueReference<T>::type;

}} // namespace bzd::typeTraits
