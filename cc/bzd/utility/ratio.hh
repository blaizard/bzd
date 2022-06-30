#pragma once

#include "cc/bzd/platform/types.hh"

namespace bzd {
template <bzd::Size Num, bzd::Size Den = 1>
class Ratio
{
public:
	static constexpr const bzd::Size num{Num};
	static constexpr const bzd::Size den{Den};
};
} // namespace bzd

namespace bzd::ratio {
using Nano = bzd::Ratio<1, 1000000000>;
using Micro = bzd::Ratio<1, 1000000>;
using Milli = bzd::Ratio<1, 1000>;
using Centi = bzd::Ratio<1, 100>;
using Deci = bzd::Ratio<1, 10>;
using Deca = bzd::Ratio<10, 1>;
using Hecto = bzd::Ratio<100, 1>;
using Kilo = bzd::Ratio<1000, 1>;
using Mega = bzd::Ratio<1000000, 1>;
using Giga = bzd::Ratio<1000000000, 1>;
} // namespace bzd::ratio
