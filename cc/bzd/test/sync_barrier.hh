#pragma once

#include "cc/bzd/core/async.hh"
#include "cc/bzd/platform/atomic.hh"
#include "cc/bzd/platform/types.hh"

namespace bzd::test {

class SyncBarrier
{
public:
	inline bzd::Async<> wait(const bzd::Size value) noexcept
	{
		co_return co_await wait(value, []() {});
	}

	template <class Callable>
	bzd::Async<> wait(const bzd::Size value, Callable callable) noexcept
	{
		bzd::Size expected{value};
		bzd::ignore = counter_.compareExchange(expected, 0u);

		++counter_;
		while (counter_.load() != value)
		{
			callable();
			co_await bzd::async::yield();
		};

		expected = value;
		bzd::ignore = sync_.compareExchange(expected, 0u);

		++sync_;
		while (sync_.load() != value)
		{
			co_await bzd::async::yield();
		};

		co_return {};
	}

private:
	bzd::Atomic<bzd::Size> counter_{0};
	bzd::Atomic<bzd::Size> sync_{0};
};

} // namespace bzd::test
