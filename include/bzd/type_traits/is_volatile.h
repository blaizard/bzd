#pragma once

#include "bzd/type_traits/false_type.h"
#include "bzd/type_traits/true_type.h"

namespace bzd { namespace typeTraits {
template <class T>
struct isVolatile : falseType
{
};
template <class T>
struct isVolatile<volatile T> : trueType
{
};
}}	// namespace bzd::typeTraits
