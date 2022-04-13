#pragma once

#include "cc/bzd/platform/types.hh"

namespace bzd::meta::impl {
template <class...>
inline constexpr BoolType alwaysFalse{false};
}

namespace bzd::meta {
using bzd::meta::impl::alwaysFalse;
}
