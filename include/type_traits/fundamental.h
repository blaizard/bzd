#pragma once

#include "include/type_traits/utils.h"
#include "include/type_traits/const_volatile.h"

namespace bzd
{
	namespace typeTraits
	{
		// isIntegral

		template <class T>
		struct isIntegral : integralConstant<bool,
				isSame<unsigned char, typename removeCV<T>::type>::value ||
                isSame<signed char, typename removeCV<T>::type>::value  ||
                isSame<char, typename removeCV<T>::type>::value ||
               	isSame<unsigned short, typename removeCV<T>::type>::value ||
                isSame<short, typename removeCV<T>::type>::value ||
                isSame<unsigned int, typename removeCV<T>::type>::value ||
                isSame<int, typename removeCV<T>::type>::value ||
                isSame<unsigned long, typename removeCV<T>::type>::value ||
                isSame<long, typename removeCV<T>::type>::value ||
                isSame<unsigned long long, typename removeCV<T>::type>::value ||
                isSame<long long, typename removeCV<T>::type>::value ||
                isSame<bool, typename removeCV<T>::type>::value> {};

		// isFloatingPoint

		template <class T>
		struct isFloatingPoint : integralConstant<bool,
                isSame<float, typename removeCV<T>::type>::value ||
                isSame<double, typename removeCV<T>::type>::value ||
                isSame<long double, typename removeCV<T>::type>::value> {};

		// isArithmetic

		template <class T>
		struct isArithmetic : integralConstant<bool, isIntegral<T>::value || isFloatingPoint<T>::value> {};
	}
}
