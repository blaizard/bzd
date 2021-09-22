#pragma once

#include "cc/bzd/meta/find_conditional.hh"
#include "cc/bzd/platform/types.hh"
#include "cc/bzd/type_traits/is_same.hh"

namespace bzd::meta {
template <class T, class... Ts>
using Find = FindConditional<bzd::typeTraits::IsSame, T, Ts...>;
} // namespace bzd::meta
