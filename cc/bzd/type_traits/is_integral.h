#pragma once

#include "bzd/type_traits/integral_constant.h"
#include "bzd/type_traits/is_same.h"
#include "bzd/type_traits/remove_cv.h"

namespace bzd::typeTraits::impl {
template <class T>
struct IsIntegral
	: IntegralConstant<bool,
					   isSame<unsigned char, RemoveCV<T>> || isSame<signed char, RemoveCV<T>> || isSame<char, RemoveCV<T>> ||
						   isSame<unsigned short, RemoveCV<T>> || isSame<short, RemoveCV<T>> || isSame<unsigned int, RemoveCV<T>> ||
						   isSame<int, RemoveCV<T>> || isSame<unsigned long, RemoveCV<T>> || isSame<long, RemoveCV<T>> ||
						   isSame<unsigned long long, RemoveCV<T>> || isSame<long long, RemoveCV<T>> || isSame<bool, RemoveCV<T>>>
{
};
} // namespace bzd::typeTraits::impl

namespace bzd::typeTraits {
template <class T>
using IsIntegral = typename impl::IsIntegral<T>;

template <class T>
constexpr bool isIntegral = IsIntegral<T>::value;

} // namespace bzd::typeTraits
