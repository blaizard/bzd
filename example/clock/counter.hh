#pragma once

#include "cc/bzd/core/delay.hh"
#include "cc/bzd/core/print.hh"

namespace example {

bzd::Async<> counter(const bzd::UInt32 count, const bzd::UInt32 periodMs)
{
	for (bzd::UInt32 n = 0u; n < count; ++n)
	{
		co_await !bzd::print("Delay {}\n"_csv, n);
		co_await !bzd::delay(bzd::units::Millisecond{periodMs});
	}

	co_return {};
}

} // namespace example
