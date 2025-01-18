#pragma once

#include "cc/bzd/core/async.hh"
#include "cc/components/esp_idf/pwm/interface.hh"

#include <driver/ledc.h>

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
	friend class LEDCPWM;

	const UInt32 frequency_;
	const ledc_timer_t number_;
};

class LEDCPWM
{
public:
	template <class Context>
	constexpr LEDCPWM(Context& context) noexcept :
		io_{context.config.io}, timer_{context.config.timer}, channel_{numberToLEDChannel<context.config.channel>()}
	{
	}

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

	bzd::Async<> init() noexcept;

	bzd::Async<> setDutyCycle(const UInt32 duty) noexcept;

	bzd::Async<> shutdown() noexcept;

private:
	const UInt32 io_;
	LEDCTimer& timer_;
	const ledc_channel_t channel_;
};

} // namespace bzd::components::esp32
