#pragma once

#include "bzd/type_traits/integral_constant.h"
#include "bzd/type_traits/is_floating_point.h"
#include "bzd/type_traits/is_integral.h"

namespace bzd::typeTraits::impl {
template <class T>
struct IsArithmetic : IntegralConstant<bool, isIntegral<T> || isFloatingPoint<T>>
{
};
} // namespace bzd::typeTraits::impl

namespace bzd::typeTraits {
template <class T>
using IsArithmetic = typename impl::IsArithmetic<T>;

template <class T>
constexpr bool isArithmetic = IsArithmetic<T>::value;
} // namespace bzd::typeTraits
