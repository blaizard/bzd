#pragma once

#include "include/type_traits/integral_constant.h"

namespace bzd
{
	namespace typeTraits
	{
		// addConst

		template <class T> struct addConst { typedef const T type; };

		// addVolatile

		template <class T> struct addVolatile { typedef volatile T type; };

		// addCV

		template <class T> struct addCV { typedef const volatile T type; };
 
		// removeConst

		template <class T> struct removeConst { typedef T type; };
		template <class T> struct removeConst<const T> { typedef T type; };
 
		// removeVolatile

		template <class T> struct removeVolatile { typedef T type; };
		template <class T> struct removeVolatile<volatile T> { typedef T type; };

		// removeCV

		template <class T>
		struct removeCV
		{
			typedef typename removeVolatile<typename removeConst<T>::type>::type type;
		};

		// isConst

		template <class T> struct isConst : falseType {};
		template <class T> struct isConst<const T> : trueType {};

		// isVolatile

		template <class T> struct isVolatile : falseType {};
		template <class T> struct isVolatile<volatile T> : trueType {};
	}
}
