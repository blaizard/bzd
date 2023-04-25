#pragma once

#include "cc/bzd/core/async.hh"
#include "cc/components/xtensa/timer/gptimer/interface.hh"
#include "cc/libs/timer/timer_isr.hh"
#include "driver/gptimer.h"

namespace bzd::components::esp32::timer {

class GPTimer
	: public bzd::Timer
	, public bzd::TimerISR<UInt64, GPTimer, /*retroactiveAlarm*/ false>
{
public:
	using Tick = UInt64;

public:
	template <class Context>
	constexpr explicit GPTimer(Context&) noexcept
	{
	}

	bzd::Async<> init() noexcept;

	bzd::Async<> shutdown() noexcept;

	bzd::Async<> delay(const bzd::units::Millisecond time) noexcept final;

	bzd::Result<bzd::units::Millisecond, bzd::Error> getTime() noexcept final;

private: // Implementation.
	friend class bzd::TimerISR<UInt64, GPTimer, /*retroactiveAlarm*/ false>;

	bzd::Result<Tick, bzd::Error> getTicks() noexcept;
	bzd::Result<void, bzd::Error> alarmSet(const Tick);
	bzd::Result<void, bzd::Error> alarmClear();

private:
	::gptimer_handle_t gptimer_;
};

} // namespace bzd::components::esp32::timer
