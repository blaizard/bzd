#pragma once

#include "cc/bzd/meta/find_conditional.h"
#include "cc/bzd/platform/types.h"
#include "cc/bzd/type_traits/is_same.h"

namespace bzd::meta {
template <class T, class... Ts>
using Find = FindConditional<bzd::typeTraits::IsSame, T, Ts...>;
} // namespace bzd::meta
