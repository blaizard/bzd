#pragma once

#include "bzd/meta/find_conditional.h"
#include "bzd/type_traits/is_same.h"
#include "bzd/types.h"

namespace bzd::meta {
template <class T, class... Ts>
using Find = FindConditional<bzd::typeTraits::IsSame, T, Ts...>;
} // namespace bzd::meta
