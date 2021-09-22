#pragma once

#include "cc/bzd/type_traits/false_type.hh"
#include "cc/bzd/type_traits/true_type.hh"

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
