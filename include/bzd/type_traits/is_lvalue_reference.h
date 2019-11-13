#pragma once

#include "bzd/type_traits/true_type.h"
#include "bzd/type_traits/false_type.h"

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
