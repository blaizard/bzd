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

		// isTriviallyDestructible

/*#if __has_keyword(__is_trivially_destructible)

		template <class T>
		struct isTriviallyDestructible : public bzd::typeTraits::integralConstant<bool, __is_trivially_destructible(T)> {};

#elif __has_feature(has_trivial_destructor)

	//	template <class T>
	//	struct isTriviallyDestructible : public bzd::typeTraits::integralConstant<bool, is_destructible<_Tp>::value && __has_trivial_destructor(T)> {};

#else
	static_assert(false, "Compiler not supported");*/
/*
template <class _Tp> struct __libcpp_trivial_destructor
    : public integral_constant<bool, is_scalar<_Tp>::value ||
                                     is_reference<_Tp>::value> {};

template <class _Tp> struct _LIBCPP_TEMPLATE_VIS is_trivially_destructible
    : public __libcpp_trivial_destructor<typename remove_all_extents<_Tp>::type> {};

template <class _Tp> struct _LIBCPP_TEMPLATE_VIS is_trivially_destructible<_Tp[]>
    : public false_type {};
*/
//#endif

	}
}
