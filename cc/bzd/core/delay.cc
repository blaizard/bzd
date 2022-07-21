#include "cc/bzd/core/delay.hh"

#include "cc/bzd/platform/clock.hh"

namespace bzd {

bzd::Async<> delay(const bzd::units::Millisecond time) noexcept
{
	auto& clock = bzd::platform::steadyClock();

	co_await !clock.delay(time);

	co_return {};
}

// NOLINTNEXTLINE(bugprone-exception-escape)
bzd::Async<> timeout(const bzd::units::Millisecond time) noexcept
{
	auto& clock = bzd::platform::steadyClock();

	co_await !clock.delay(time);

	co_return bzd::error::Timeout("Operation timed out after {}ms"_csv, time.get());
}

} // namespace bzd
