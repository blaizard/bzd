#pragma once

#include "include/type_traits/integral_constant.h"

namespace bzd
{
	namespace typeTraits
	{
		// removeExtent

		template <class T> struct removeExtent { typedef T type; };
 		template <class T> struct removeExtent<T[]> { typedef T type; };
		template <class T, unsigned long int N> struct removeExtent<T[N]> { typedef T type; };

		// isArray

		template <class T> struct isArray : falseType {};
 		template <class T> struct isArray<T[]> : trueType {};
		template <class T, unsigned long int N> struct isArray<T[N]> : trueType {};
	}
}
