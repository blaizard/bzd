#pragma once

#include "cc/bzd/type_traits/integral_constant.hh"
#include "cc/bzd/type_traits/is_floating_point.hh"
#include "cc/bzd/type_traits/is_integral.hh"

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
inline constexpr bool isArithmetic = IsArithmetic<T>::value;
} // namespace bzd::typeTraits

namespace bzd::concepts {
template <class T>
concept arithmetic = typeTraits::isArithmetic<T>;
}
