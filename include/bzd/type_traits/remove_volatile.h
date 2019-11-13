#pragma once

#include "bzd/type_traits/true_type.h"
#include "bzd/type_traits/false_type.h"

namespace bzd
{
	namespace typeTraits
	{
		template <class T> struct removeVolatile { typedef T type; };
		template <class T> struct removeVolatile<volatile T> { typedef T type; };
	}
}
