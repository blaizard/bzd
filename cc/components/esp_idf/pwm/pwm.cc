#include "cc/components/esp_idf/pwm/pwm.hh"

#include "cc/components/esp_idf/error.hh"

#include <driver/ledc.h>

namespace bzd::components::esp32 {

#define LEDC_MODE LEDC_LOW_SPEED_MODE
#define LEDC_CHANNEL LEDC_CHANNEL_0
#define LEDC_DUTY_RES LEDC_TIMER_13_BIT // Set duty resolution to 13 bits
#define LEDC_DUTY (1000)				// Set duty to 50%. (2 ** 13) * 50% = 4096
#define LEDC_FREQUENCY (4000)			// Frequency in Hertz. Set frequency at 4 kHz

bzd::Async<> LEDCTimer::init() noexcept
{
	::ledc_timer_config_t ledc_timer{};
	ledc_timer.speed_mode = LEDC_LOW_SPEED_MODE;
	ledc_timer.duty_resolution = LEDC_TIMER_13_BIT;
	ledc_timer.timer_num = number_;
	ledc_timer.freq_hz = frequency_;
	ledc_timer.clk_cfg = LEDC_AUTO_CLK;

	if (const auto result = ::ledc_timer_config(&ledc_timer); result != ESP_OK)
	{
		co_return bzd::error::EspErr("ledc_timer_config", result);
	}

	co_return {};
}

bzd::Async<> LEDCTimer::shutdown() noexcept { co_return {}; }

bzd::Async<> LEDCPWM::init() noexcept
{
	// Prepare and then apply the LEDC PWM channel configuration
	::ledc_channel_config_t ledc_channel{};
	ledc_channel.speed_mode = LEDC_MODE;
	ledc_channel.channel = channel_;
	ledc_channel.timer_sel = timer_.number_;
	ledc_channel.intr_type = LEDC_INTR_DISABLE;
	ledc_channel.gpio_num = io_;
	ledc_channel.duty = 0; // Set duty to 0%
	ledc_channel.hpoint = 0;

	if (const auto result = ::ledc_channel_config(&ledc_channel); result != ESP_OK)
	{
		co_return bzd::error::EspErr("ledc_channel_config", result);
	}

	co_return {};
}

bzd::Async<> LEDCPWM::shutdown() noexcept { co_return {}; }

bzd::Async<> LEDCPWM::setDutyCycle(const UInt32 duty) noexcept
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

} // namespace bzd::components::esp32
