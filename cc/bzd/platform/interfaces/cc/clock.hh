#pragma once

#include "cc/bzd/core/async.hh"
#include "cc/bzd/core/error.hh"
#include "cc/bzd/core/units.hh"
#include "cc/bzd/platform/types.hh"
#include "tools/bdl/generators/cc/adapter/types.hh"

namespace bzd {

class Clock
{
public:
	virtual bzd::Async<> waitUntil(const bzd::units::Millisecond time) noexcept
	{
		bzd::units::Millisecond current{};
		do
		{
			co_await bzd::async::yield();
			auto maybeCurrent = getTime();
			if (!maybeCurrent)
			{
				co_return bzd::move(maybeCurrent).propagate();
			}
			current = maybeCurrent.value();
		} while (current < time);

		co_return {};
	}

	virtual bzd::Result<bzd::units::Millisecond, bzd::Error> getTime() noexcept = 0;
};

} // namespace bzd
