#pragma once

#include "bzd/type_traits/false_type.h"
#include "bzd/type_traits/true_type.h"

namespace bzd { namespace typeTraits {
namespace impl {
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
} // namespace impl

template <class T>
using RemoveVolatile = typename impl::RemoveVolatile<T>::type;

}} // namespace bzd::typeTraits
