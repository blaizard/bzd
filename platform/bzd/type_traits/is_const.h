#pragma once

#include "bzd/type_traits/false_type.h"
#include "bzd/type_traits/true_type.h"

namespace bzd { namespace typeTraits {
template <class T>
struct isConst : falseType
{
};
template <class T>
struct isConst<const T> : trueType
{
};
}} // namespace bzd::typeTraits
