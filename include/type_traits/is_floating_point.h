#pragma once

#include "include/type_traits/integral_constant.h"
#include "include/type_traits/remove_cv.h"

namespace bzd
{
	namespace typeTraits
	{
		template <class T>
		struct isFloatingPoint : integralConstant<bool,
                isSame<float, typename removeCV<T>::type>::value ||
                isSame<double, typename removeCV<T>::type>::value ||
                isSame<long double, typename removeCV<T>::type>::value> {};
	}
}
