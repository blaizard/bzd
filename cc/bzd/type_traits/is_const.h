#pragma once

#include "cc/bzd/type_traits/false_type.h"
#include "cc/bzd/type_traits/true_type.h"

namespace bzd::typeTraits::impl {
template <class T>
struct IsConst : FalseType
{
};
template <class T>
struct IsConst<const T> : TrueType
{
};
} // namespace bzd::typeTraits::impl

namespace bzd::typeTraits {
template <class T>
using IsConst = typename impl::IsConst<T>;

template <class T>
constexpr bool isConst = IsConst<T>::value;
} // namespace bzd::typeTraits
