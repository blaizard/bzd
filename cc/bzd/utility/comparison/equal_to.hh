#pragma once

#include "cc/bzd/platform/types.hh"

namespace bzd {
template <class T>
struct EqualTo
{
	constexpr BoolType operator()(const T& lhs, const T& rhs) const { return lhs == rhs; }
};
} // namespace bzd
