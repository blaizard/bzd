#pragma once

#include "bzd/type_traits/integral_constant.h"
#include "bzd/type_traits/is_floating_point.h"
#include "bzd/type_traits/is_integral.h"

namespace bzd { namespace typeTraits {
template <class T>
struct isArithmetic : integralConstant<bool, isIntegral<T>::value || isFloatingPoint<T>::value>
{
};
}} // namespace bzd::typeTraits
