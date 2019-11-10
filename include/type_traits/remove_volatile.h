#pragma once

#include "include/type_traits/true_type.h"
#include "include/type_traits/false_type.h"

namespace bzd
{
	namespace typeTraits
	{
		template <class T> struct removeVolatile { typedef T type; };
		template <class T> struct removeVolatile<volatile T> { typedef T type; };
	}
}
