#pragma once

#include "include/type_traits/integral_constant.h"
#include "include/type_traits/void.h"

namespace bzd
{
	namespace typeTraits
	{
		// isLValueReference

		template <class T>
		struct isLValueReference : public falseType {};

		template <class T>
		struct isLValueReference<T&> : public trueType {};

		// isRValueReference

		template <class T>
		struct isRValueReference : public falseType {};

		template <class T>
		struct isRValueReference<T&&> : public trueType {};

		// isReference

		template <class T>
		struct isReference : public integralConstant<bool, isLValueReference<T>::value || isRValueReference<T>::value> {};

		// removeReference

		template <class T>
		struct removeReference { typedef T type; };

		template <class T>
		struct removeReference<T&> { typedef T type; };

		template <class T>
		struct removeReference<T&&> { typedef T type; };

		// addReference

		template <class T>
		struct addReference { typedef T& type; };

		template <class T>
		struct addReference<T&> { typedef T& type; };

		template <class T>
		struct addReference<T&&> { typedef T&& type; };

		template <>
		struct addReference<void> { typedef void type; };
		template <>
		struct addReference<const void> { typedef const void type; };
		template <>
		struct addReference<const volatile void> { typedef const volatile void type; };
		template <>
		struct addReference<volatile void> { typedef volatile void type; };

		// addRValueReference

		namespace impl
		{
			template <typename T, bool b>
			struct addRValueReferenceHelper { typedef T type; };

			template <typename T>
			struct addRValueReferenceHelper<T, true> { typedef T&& type; };

			template <typename T>
			struct addRValueReference
			{
				typedef typename addRValueReferenceHelper<T, (isVoid<T>::value == false && isReference<T>::value == false) >::type type;
			};
		}

		template <class T>
		struct addRValueReference { typedef typename impl::addRValueReference<T>::type type; };

		// addLValueReference

		template <class T>
		struct addLValueReference { typedef typename addReference<T>::type type; };

		template <class T>
		struct addLValueReference<T&&> { typedef T& type; };
	}
}
