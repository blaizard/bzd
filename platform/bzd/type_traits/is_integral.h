#pragma once

#include "bzd/type_traits/integral_constant.h"
#include "bzd/type_traits/is_same.h"
#include "bzd/type_traits/remove_cv.h"

namespace bzd { namespace typeTraits { namespace impl {
template <class T>
struct IsIntegral
	: integralConstant<bool,
					   isSame<unsigned char, typename removeCV<T>::type> || isSame<signed char, typename removeCV<T>::type> ||
						   isSame<char, typename removeCV<T>::type> || isSame<unsigned short, typename removeCV<T>::type> ||
						   isSame<short, typename removeCV<T>::type> || isSame<unsigned int, typename removeCV<T>::type> ||
						   isSame<int, typename removeCV<T>::type> || isSame<unsigned long, typename removeCV<T>::type> ||
						   isSame<long, typename removeCV<T>::type> ||
						   isSame<unsigned long long, typename removeCV<T>::type> ||
						   isSame<long long, typename removeCV<T>::type> || isSame<bool, typename removeCV<T>::type>>
{
};
}

template <class T>
using IsIntegral = typename impl::IsIntegral<T>;

template <class T>
constexpr bool isIntegral = IsIntegral<T>::value;

}} // namespace bzd::typeTraits
