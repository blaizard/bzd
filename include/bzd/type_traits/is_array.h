#pragma once

#include "bzd/type_traits/true_type.h"
#include "bzd/type_traits/false_type.h"

namespace bzd
{
	namespace typeTraits
	{
		template <class T> struct isArray : bzd::typeTraits::falseType {};
 		template <class T> struct isArray<T[]> : bzd::typeTraits::trueType {};
		template <class T, unsigned long int N> struct isArray<T[N]> : bzd::typeTraits::trueType {};
	}
}
