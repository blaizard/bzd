#pragma once

#include "cc/bzd/type_traits/integral_constant.hh"
#include "cc/bzd/type_traits/is_same.hh"
#include "cc/bzd/type_traits/remove_cv.hh"

namespace bzd::typeTraits::impl {
template <class T>
struct IsFloatingPoint
	: IntegralConstant<bool, isSame<float, RemoveCV<T>> || isSame<double, RemoveCV<T>> || isSame<long double, RemoveCV<T>>>
{
};
} // namespace bzd::typeTraits::impl
namespace bzd::typeTraits {
template <class T>
using IsFloatingPoint = typename impl::IsFloatingPoint<T>;

template <class T>
constexpr bool isFloatingPoint = IsFloatingPoint<T>::value;

} // namespace bzd::typeTraits