#pragma once

#include "include/type_traits/true_type.h"
#include "include/type_traits/false_type.h"

namespace bzd
{
	namespace typeTraits
	{
		template <class T>
		struct isRValueReference : public falseType {};

		template <class T>
		struct isRValueReference<T&&> : public trueType {};
	}
}
