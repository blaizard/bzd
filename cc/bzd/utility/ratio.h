#pragma once

#include "bzd/platform/types.h"

namespace bzd {
template <bzd::SizeType Num, bzd::SizeType Den = 1>
class Ratio
{
public:
	static constexpr const bzd::SizeType num{Num};
	static constexpr const bzd::SizeType den{Den};
};
} // namespace bzd
