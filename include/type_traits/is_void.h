#pragma once

#include "include/type_traits/true_type.h"
#include "include/type_traits/false_type.h"

namespace bzd
{
	namespace typeTraits
	{
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
	}
}
