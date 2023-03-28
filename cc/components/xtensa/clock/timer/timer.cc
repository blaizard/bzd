#include "cc/components/xtensa/clock/timer/timer.hh"

#include "cc/components/xtensa/error.hh"
#include "sdkconfig.h"

namespace bzd::platform::esp32::clock {

static bool gptimerCallback(::gptimer_handle_t, const ::gptimer_alarm_event_data_t*, void* userCtx)
{
	Timer& timer = *static_cast<Timer*>(userCtx);
	timer.triggerForISR();
	// return whether we need to yield at the end of ISR
	return false;
}

bzd::Async<> Timer::init() noexcept
{
	const ::gptimer_config_t config{.clk_src = GPTIMER_CLK_SRC_APB,
									.direction = GPTIMER_COUNT_UP,
									// Note: to avoid an assert, (counter_src_hz / resolution_hz) must be >= 2 && <= 65536
									.resolution_hz = 10 * 1000, // 0.1ms
									.flags = {.intr_shared = false}};

	if (const auto result = ::gptimer_new_timer(&config, &gptimer_); result != ESP_OK)
	{
		co_return bzd::error::EspErr("gptimer_new_timer", result);
	}
	const ::gptimer_event_callbacks_t callback{.on_alarm = gptimerCallback};
	if (const auto result = ::gptimer_register_event_callbacks(gptimer_, &callback, this); result != ESP_OK)
	{
		co_return bzd::error::EspErr("gptimer_register_event_callbacks", result);
	}
	if (const auto result = ::gptimer_enable(gptimer_); result != ESP_OK)
	{
		co_return bzd::error::EspErr("gptimer_enable", result);
	}
	if (const auto result = ::gptimer_start(gptimer_); result != ESP_OK)
	{
		co_return bzd::error::EspErr("gptimer_start", result);
	}

	co_return {};
}

bzd::Async<> Timer::shutdown() noexcept
{
	if (const auto result = ::gptimer_stop(gptimer_); result != ESP_OK)
	{
		co_return bzd::error::EspErr("gptimer_stop", result);
	}
	if (const auto result = ::gptimer_disable(gptimer_); result != ESP_OK)
	{
		co_return bzd::error::EspErr("gptimer_disable", result);
	}
	if (const auto result = ::gptimer_del_timer(gptimer_); result != ESP_OK)
	{
		co_return bzd::error::EspErr("gptimer_del_timer", result);
	}

	co_return {};
}

bzd::Async<> Timer::delay(const bzd::units::Millisecond duration) noexcept
{
	auto maybeTime = getTime_();
	if (!maybeTime)
	{
		co_return bzd::move(maybeTime).propagate();
	}
	co_await !waitUntil_(maybeTime.value() + duration.get() * 10);
	co_return {};
}

bzd::Result<bzd::units::Millisecond, bzd::Error> Timer::getTime() noexcept
{
	// TODO: To be updated, the / 10 is inefficient here.
	auto maybeTime = getTime_();
	if (!maybeTime)
	{
		return bzd::move(maybeTime).propagate();
	}
	return bzd::units::Millisecond(maybeTime.value() / 10);
}

bzd::Result<Timer::Time, bzd::Error> Timer::getTime_() noexcept
{
	Timer::Time count{};
	if (const auto result = ::gptimer_get_raw_count(gptimer_, &count); result != ESP_OK)
	{
		return bzd::error::EspErr("gptimer_get_raw_count", result);
	}
	return count;
}

bzd::Result<void, bzd::Error> Timer::alarmSet_(const Time time)
{
	const ::gptimer_alarm_config_t config{.alarm_count = time, .reload_count = 0, .flags = {.auto_reload_on_alarm = false}};
	if (const auto result = ::gptimer_set_alarm_action(gptimer_, &config); result != ESP_OK)
	{
		return bzd::error::EspErr("gptimer_set_alarm_action", result);
	}
	return bzd::nullresult;
}

bzd::Result<void, bzd::Error> Timer::alarmClear_()
{
	if (const auto result = ::gptimer_set_alarm_action(gptimer_, nullptr); result != ESP_OK)
	{
		return bzd::error::EspErr("gptimer_set_alarm_action", result);
	}
	return bzd::nullresult;
}

} // namespace bzd::platform::esp32::clock
