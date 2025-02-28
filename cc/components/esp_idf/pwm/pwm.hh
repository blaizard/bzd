#pragma once

#include "cc/bzd/core/async.hh"
#include "cc/components/esp_idf/error.hh"
#include "cc/components/esp_idf/pwm/interface.hh"

#include <driver/ledc.h>

#define LEDC_MODE LEDC_LOW_SPEED_MODE
#define LEDC_CHANNEL LEDC_CHANNEL_0
#define LEDC_DUTY_RES LEDC_TIMER_13_BIT // Set duty resolution to 13 bits
#define LEDC_DUTY (1000)				// Set duty to 50%. (2 ** 13) * 50% = 4096
#define LEDC_FREQUENCY (4000)			// Frequency in Hertz. Set frequency at 4 kHz

namespace bzd::components::esp32 {

class LEDCTimer
{
public:
	template <class Context>
	constexpr LEDCTimer(Context& context) noexcept :
		frequency_{context.config.frequency}, number_{numberToLEDCTimer<context.config.number>()}
	{
	}

	template <auto number>
	static constexpr ledc_timer_t numberToLEDCTimer() noexcept
	{
		static_assert(number >= 0 && number <= LEDC_TIMER_MAX, "LEDC timer number not in valid range for this target.");
		switch (number)
		{
		case 0:
			return LEDC_TIMER_0;
		case 1:
			return LEDC_TIMER_1;
		case 2:
			return LEDC_TIMER_2;
		case 3:
			return LEDC_TIMER_3;
		}
	}

	bzd::Async<> init() noexcept;

	bzd::Async<> shutdown() noexcept;

private:
	template <class>
	friend class LEDCPWM;

	const UInt32 frequency_;
	const ledc_timer_t number_;
};

template <class Context>
class LEDCPWM
{
public:
	constexpr LEDCPWM(Context& context) noexcept : context_{context}, channel_{numberToLEDChannel<context.config.channel>()} {}

	template <auto number>
	static constexpr ledc_channel_t numberToLEDChannel() noexcept
	{
		static_assert(number >= 0 && number <= LEDC_CHANNEL_MAX, "LEDC channel number not in valid range for this target.");
		switch (number)
		{
		case 0:
			return LEDC_CHANNEL_0;
		case 1:
			return LEDC_CHANNEL_1;
		case 2:
			return LEDC_CHANNEL_2;
		case 3:
			return LEDC_CHANNEL_3;
		case 4:
			return LEDC_CHANNEL_4;
		case 5:
			return LEDC_CHANNEL_5;
		case 6:
			return LEDC_CHANNEL_6;
		case 7:
			return LEDC_CHANNEL_7;
		}
	}

	bzd::Async<> init() noexcept
	{
		::ledc_channel_config_t ledc_channel{};
		ledc_channel.speed_mode = LEDC_MODE;
		ledc_channel.channel = channel_;
		ledc_channel.timer_sel = context_.config.timer.number_;
		ledc_channel.intr_type = LEDC_INTR_DISABLE;
		ledc_channel.gpio_num = context_.config.pin;
		ledc_channel.duty = 0;
		ledc_channel.hpoint = 0;

		if (const auto result = ::ledc_channel_config(&ledc_channel); result != ESP_OK)
		{
			co_return bzd::error::EspErr("ledc_channel_config", result);
		}

		co_return {};
	}

	bzd::Async<> run() noexcept
	{
		while (true)
		{
			const auto result = co_await !context_.io.duty.getNew();
			co_await !setDutyCycle(result.value());
		}
		co_return {};
	}

	bzd::Async<> shutdown() noexcept
	{
		if (const auto result = ::ledc_stop(LEDC_MODE, channel_, 0u); result != ESP_OK)
		{
			co_return bzd::error::EspErr("ledc_stop", result);
		}
		co_return {};
	}

private:
	bzd::Async<> setDutyCycle(const UInt32 duty) noexcept
	{
		if (const auto result = ::ledc_set_duty(LEDC_MODE, channel_, duty); result != ESP_OK)
		{
			co_return bzd::error::EspErr("ledc_set_duty", result);
		}

		if (const auto result = ::ledc_update_duty(LEDC_MODE, channel_); result != ESP_OK)
		{
			co_return bzd::error::EspErr("ledc_update_duty", result);
		}

		co_return {};
	}

private:
	Context& context_;
	const ledc_channel_t channel_;
};

} // namespace bzd::components::esp32
