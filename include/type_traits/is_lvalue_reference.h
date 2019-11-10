#pragma once

#include "include/type_traits/true_type.h"
#include "include/type_traits/false_type.h"

namespace bzd
{
	namespace typeTraits
	{
		template <class T>
		struct isLValueReference : public falseType {};

		template <class T>
		struct isLValueReference<T&> : public trueType {};
	}
}
