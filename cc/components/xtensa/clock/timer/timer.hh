#pragma once

#include "cc/bzd/core/async.hh"
#include "cc/bzd/core/clock.hh"
#include "cc/libs/timer/timer_isr.hh"
#include "driver/gptimer.h"

namespace bzd::platform::esp32::clock {

class Timer
	: public bzd::Clock
	, public bzd::TimerISR<UInt64, Timer, /*retroactiveAlarm*/ false>
{
public:
	using Time = UInt64;

public:
	template <class Context>
	constexpr explicit Timer(Context&) noexcept
	{
	}

	bzd::Async<> init() noexcept;

	bzd::Async<> shutdown() noexcept;

	ClockTick getTicks() noexcept final;

	ClockTick msToTicks(const bzd::units::Millisecond time) noexcept final;

	bzd::units::Millisecond ticksToMs(const ClockTick& ticks) noexcept final;

	bzd::Async<> delay(const bzd::units::Millisecond time) noexcept final;

private: // Implementation.
	friend class bzd::TimerISR<UInt64, Timer, /*retroactiveAlarm*/ false>;

	[[nodiscard]] bzd::Result<Timer::Time, bzd::Error> getTime() noexcept;

	bzd::Result<void, bzd::Error> alarmSet(const Time);
	bzd::Result<void, bzd::Error> alarmClear();

private:
	::gptimer_handle_t gptimer_;
};

} // namespace bzd::platform::esp32::clock
