#include "cc/components/esp_idf/timer/gptimer/gptimer.hh"

#include "cc/components/esp_idf/error.hh"
#include "sdkconfig.h"

namespace bzd::components::esp32::timer {

static bool gptimerCallbackISR(::gptimer_handle_t, const ::gptimer_alarm_event_data_t*, void* userCtx)
{
	GPTimer& timer = *static_cast<GPTimer*>(userCtx);
	timer.triggerForISR();
	// return whether we need to yield at the end of ISR
	return false;
}

bzd::Async<> GPTimer::init() noexcept
{
	::gptimer_config_t config{};
	config.clk_src = GPTIMER_CLK_SRC_APB;
	config.direction = GPTIMER_COUNT_UP;
	// Note: to avoid an assert, (counter_src_hz / resolution_hz) must be >= 2 && <= 65536
	config.resolution_hz = 10 * 1000; // 0.1ms
	config.flags.intr_shared = false; // Not mark the timer interrupt source as a shared one.

	if (const auto result = ::gptimer_new_timer(&config, &gptimer_); result != ESP_OK)
	{
		co_return bzd::error::EspErr("gptimer_new_timer", result);
	}
	const ::gptimer_event_callbacks_t callback{.on_alarm = gptimerCallbackISR};
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

bzd::Async<> GPTimer::shutdown() noexcept
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

bzd::Async<> GPTimer::delay(const bzd::units::Millisecond duration) noexcept
{
	auto maybeTime = getTicks();
	if (!maybeTime)
	{
		co_return bzd::move(maybeTime).propagate();
	}
	co_await !waitUntilTicks(maybeTime.value() + duration.get() * 10);
	co_return {};
}

bzd::Result<bzd::units::Millisecond, bzd::Error> GPTimer::getTime() noexcept
{
	// TODO: To be updated, the / 10 is inefficient here.
	auto maybeTime = getTicks();
	if (!maybeTime)
	{
		return bzd::move(maybeTime).propagate();
	}
	return bzd::units::Millisecond(maybeTime.value() / 10);
}

bzd::Result<GPTimer::Tick, bzd::Error> GPTimer::getTicks() noexcept
{
	Tick count{};
	if (const auto result = ::gptimer_get_raw_count(gptimer_, &count); result != ESP_OK)
	{
		return bzd::error::EspErr("gptimer_get_raw_count", result);
	}
	return count;
}

bzd::Result<void, bzd::Error> GPTimer::alarmSet(const Tick time)
{
	const ::gptimer_alarm_config_t config{.alarm_count = time, .reload_count = 0, .flags = {.auto_reload_on_alarm = false}};
	if (const auto result = ::gptimer_set_alarm_action(gptimer_, &config); result != ESP_OK)
	{
		return bzd::error::EspErr("gptimer_set_alarm_action", result);
	}
	return bzd::nullresult;
}

bzd::Result<void, bzd::Error> GPTimer::alarmClear()
{
	if (const auto result = ::gptimer_set_alarm_action(gptimer_, nullptr); result != ESP_OK)
	{
		return bzd::error::EspErr("gptimer_set_alarm_action", result);
	}
	return bzd::nullresult;
}

} // namespace bzd::components::esp32::timer
