#pragma once

#include "cc/bzd/core/async.hh"
#include "cc/components/xtensa/clock/timer/interface.hh"
#include "cc/libs/timer/timer_isr.hh"
#include "driver/gptimer.h"

namespace bzd::platform::esp32::clock {

class Timer
	: public bzd::Clock2
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

	bzd::Async<> delay(const bzd::units::Millisecond time) noexcept final;

	bzd::Result<bzd::units::Millisecond, bzd::Error> getTime() noexcept final;

private: // Implementation.
	friend class bzd::TimerISR<UInt64, Timer, /*retroactiveAlarm*/ false>;

	bzd::Result<Timer::Time, bzd::Error> getTime_() noexcept;
	bzd::Result<void, bzd::Error> alarmSet_(const Time);
	bzd::Result<void, bzd::Error> alarmClear_();

private:
	::gptimer_handle_t gptimer_;
};

} // namespace bzd::platform::esp32::clock
