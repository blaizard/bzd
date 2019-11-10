#pragma once

#include "include/type_traits/add_rvalue_reference.h"

namespace bzd
{
	namespace typeTraits
	{
		template <class T>
		typename addRValueReference<T>::type declval();
	}
}
