#pragma once

#include "include/type_traits/integral_constant.h"
#include "include/type_traits/reference.h"

namespace bzd
{
	namespace typeTraits
	{
		// declval

		template <class T>
		typename addRValueReference<T>::type declval();

		// enableIf

		template <bool B, class T = void>
		struct enableIf {};
	
		template <class T>
		struct enableIf<true, T> { typedef T type; };

		// conditional

		template<bool B, class T, class F>
		struct conditional { typedef T type; };

		template<class T, class F>
		struct conditional<false, T, F> { typedef F type; };

		// isSame

		template <class T, class U>
		struct isSame : falseType {};

		template <class T>
		struct isSame<T, T> : trueType {};
	}
}
