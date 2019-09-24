#pragma once

#include "include/type_traits/integral_constant.h"

namespace bzd
{
	namespace typeTraits
	{
		// isVoid

		template <class T>
		struct isVoid : public falseType {};

		template<>
		struct isVoid<void> : public trueType {};

		template<>
		struct isVoid<const void> : public trueType {};

		template<>
		struct isVoid<const volatile void> : public trueType {};

		template<>
		struct isVoid<volatile void> : public trueType {};

		// voidType

		template <class...>
		using voidType = void;
	}
}
