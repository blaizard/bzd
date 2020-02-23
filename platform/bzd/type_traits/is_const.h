#pragma once

#include "bzd/type_traits/false_type.h"
#include "bzd/type_traits/true_type.h"

namespace bzd { namespace typeTraits { namespace impl {
template <class T>
struct IsConst : falseType
{
};
template <class T>
struct IsConst<const T> : trueType
{
};
}

template <class T>
using IsConst = typename impl::IsConst<T>;

template <class T>
constexpr bool isConst = IsConst<T>::value;

}} // namespace bzd::typeTraits
