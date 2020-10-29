#pragma once

#include "bzd/type_traits/false_type.h"
#include "bzd/type_traits/true_type.h"

namespace bzd::typeTraits::impl {
template <class T>
struct RemoveVolatile
{
	typedef T type;
};
template <class T>
struct RemoveVolatile<volatile T>
{
	typedef T type;
};
} // namespace bzd::typeTraits::impl

namespace bzd::typeTraits {
template <class T>
using RemoveVolatile = typename impl::RemoveVolatile<T>::type;

} // namespace bzd::typeTraits
