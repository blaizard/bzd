#pragma once

#include "bzd/type_traits/integral_constant.h"
#include "bzd/type_traits/is_same.h"
#include "bzd/type_traits/remove_cv.h"

namespace bzd { namespace typeTraits {
namespace impl {
template <class T>
struct IsFloatingPoint
	: integralConstant<bool, isSame<float, RemoveCV<T>> || isSame<double, RemoveCV<T>> || isSame<long double, RemoveCV<T>>>
{
};
} // namespace impl

template <class T>
using IsFloatingPoint = typename impl::IsFloatingPoint<T>;

template <class T>
constexpr bool isFloatingPoint = IsFloatingPoint<T>::value;

}} // namespace bzd::typeTraits
