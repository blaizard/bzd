#pragma once

#include "bzd/type_traits/integral_constant.h"
#include "bzd/type_traits/is_lvalue_reference.h"
#include "bzd/type_traits/is_rvalue_reference.h"

namespace bzd
{
	namespace typeTraits
	{
		template <class T>
		struct isReference : public integralConstant<bool, isLValueReference<T>::value || isRValueReference<T>::value> {};
	}
}
