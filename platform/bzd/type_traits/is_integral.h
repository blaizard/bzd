#pragma once

#include "bzd/type_traits/integral_constant.h"
#include "bzd/type_traits/is_same.h"
#include "bzd/type_traits/remove_cv.h"

namespace bzd { namespace typeTraits {
template <class T>
struct isIntegral
	: integralConstant<bool,
					   isSame<unsigned char, typename removeCV<T>::type>::value || isSame<signed char, typename removeCV<T>::type>::value ||
						   isSame<char, typename removeCV<T>::type>::value || isSame<unsigned short, typename removeCV<T>::type>::value ||
						   isSame<short, typename removeCV<T>::type>::value || isSame<unsigned int, typename removeCV<T>::type>::value ||
						   isSame<int, typename removeCV<T>::type>::value || isSame<unsigned long, typename removeCV<T>::type>::value ||
						   isSame<long, typename removeCV<T>::type>::value ||
						   isSame<unsigned long long, typename removeCV<T>::type>::value ||
						   isSame<long long, typename removeCV<T>::type>::value || isSame<bool, typename removeCV<T>::type>::value>
{
};
}} // namespace bzd::typeTraits
