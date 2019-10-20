#pragma once

#include "include/type_traits/integral_constant.h"
#include "include/type_traits/reference.h"
#include "include/type_traits/void.h"
#include "include/type_traits/utils.h"

#ifndef __is_identifier
#define __is_identifier(__x) 1
#endif
#define __has_keyword(__x) !(__is_identifier(__x))
#ifndef __has_feature
#define __has_feature(__x) 0
#endif

namespace bzd
{
	namespace typeTraits
	{
		// isDestructible

		namespace impl
		{
			struct isDestructible
			{
				template <class T, class = decltype(bzd::typeTraits::declval<T&>().~T())>
				static char test(int);
				template <class>
				static long int test(...);
			};
		}

		template <class T>
		struct isDestructible : public bzd::typeTraits::integralConstant<bool, (sizeof(impl::isDestructible::test<T>(0)) == sizeof(char))> {};

		template <> struct isDestructible<void> : public falseType {};
		template <> struct isDestructible<void const> : public falseType {};
		template <> struct isDestructible<void volatile> : public falseType {};
		template <> struct isDestructible<void const volatile> : public falseType {};
		template <class T> struct isDestructible<T&> : public isDestructible<T> {};
		template <class T> struct isDestructible<T&&> : public isDestructible<T> {};
		template <class T, unsigned long int N> struct isDestructible<T[N]> : public isDestructible<T> {};
		template <class T> struct isDestructible<T[]> : public isDestructible<T> {};

#if __has_keyword(__is_trivially_destructible)
		template <class T>
		struct isTriviallyDestructible : public bzd::typeTraits::integralConstant<bool, __is_trivially_destructible(T)> {};
#elif __has_feature(has_trivial_destructor) || defined(__GNUC__)
		template <class T>
		struct isTriviallyDestructible : public bzd::typeTraits::integralConstant<bool, isDestructible<T>::value && __has_trivial_destructor(T)> {};
#else
		static_assert(false, "Unsupported compiler");
#endif

	}
}
