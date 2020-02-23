#pragma once

#include "bzd/type_traits/integral_constant.h"
#include "bzd/type_traits/is_floating_point.h"
#include "bzd/type_traits/is_integral.h"

namespace bzd { namespace typeTraits {
namespace impl {
template <class T>
struct IsArithmetic : integralConstant<bool, isIntegral<T> || isFloatingPoint<T>>
{
};
} // namespace impl

template <class T>
using IsArithmetic = typename impl::IsArithmetic<T>;

template <class T>
constexpr bool isArithmetic = IsArithmetic<T>::value;

}} // namespace bzd::typeTraits
