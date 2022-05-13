#pragma once

#include "cc/bzd/core/async.hh"
#include "cc/bzd/core/units.hh"
#include "cc/bzd/platform/clock.hh"

namespace bzd {

/// Pauses the program for the amount of time (in milliseconds) specified as parameter.
///
/// \param time Time to pause the program for.
/// \return A promise object.
inline bzd::Async<> delay(const bzd::units::Millisecond time) noexcept
{
	auto& clock = bzd::platform::steadyClock();

	co_await !clock.delay(time);

	co_return {};
}

/// Pauses the program for the amount of time (in milliseconds) specified as parameter and returns a timeout error.
///
/// \param time Time to pause the program for.
/// \return A promise object.
inline bzd::Async<> timeout(const bzd::units::Millisecond time) noexcept
{
	auto& clock = bzd::platform::steadyClock();

	co_await !clock.delay(time);

	co_return bzd::error(ErrorType::timeout, "Operation timed out with {}ms"_csv, time.get());
}

} // namespace bzd
