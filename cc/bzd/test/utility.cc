#include "cc/bzd/test/utility.hh"

namespace bzd::test {

bzd::Async<> delay(const bzd::Size count) noexcept
{
	for (bzd::Size i = 0; i < count; ++i)
	{
		co_await bzd::async::yield();
	}
	co_return {};
}

// NOLINTNEXTLINE(bugprone-exception-escape)
bzd::Async<> timeout(const bzd::Size count) noexcept
{
	co_await !delay(count);
	co_return bzd::error::Timeout("Operation timed out after {} ticks"_csv, count);
}

} // namespace bzd::test
