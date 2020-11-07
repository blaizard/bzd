#pragma once

#include "bzd/type_traits/integral_constant.h"
#include "bzd/type_traits/remove_cv.h"

namespace bzd::typeTraits::impl {
template <class T>
struct IsEnum : public bzd::typeTraits::IntegralConstant<bool, __is_enum(RemoveCV<T>)>
{
};
} // namespace bzd::typeTraits::impl

namespace bzd::typeTraits {
template <class T>
using IsEnum = typename impl::IsEnum<T>;

template <class T>
constexpr bool isEnum = IsEnum<T>::value;
} // namespace bzd::typeTraits
