#pragma once

#include "cc/bzd/type_traits/integral_constant.hh"
#include "cc/bzd/type_traits/remove_cv.hh"

namespace bzd::typeTraits::impl {
template <class T>
struct IsUnion : public bzd::typeTraits::IntegralConstant<bool, __is_union(RemoveCV<T>)>
{
};
} // namespace bzd::typeTraits::impl

namespace bzd::typeTraits {
template <class T>
using IsUnion = typename impl::IsUnion<T>;

template <class T>
inline constexpr bool isUnion = IsUnion<T>::value;
} // namespace bzd::typeTraits
