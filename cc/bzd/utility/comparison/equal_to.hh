#pragma once

#include "cc/bzd/platform/types.hh"

namespace bzd {
template <class T1, class T2 = T1>
struct EqualTo
{
	constexpr Bool operator()(const T1& lhs, const T2& rhs) const { return lhs == rhs; }
};
} // namespace bzd
