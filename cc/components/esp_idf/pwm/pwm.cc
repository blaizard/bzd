#include "cc/components/esp_idf/pwm/pwm.hh"

namespace bzd::components::esp32 {

bzd::Async<> LEDCTimer::init() noexcept
{
	::ledc_timer_config_t ledc_timer{};
	ledc_timer.speed_mode = LEDC_MODE;
	ledc_timer.duty_resolution = LEDC_TIMER_13_BIT;
	ledc_timer.timer_num = number_;
	ledc_timer.freq_hz = frequency_;
	ledc_timer.clk_cfg = LEDC_AUTO_CLK;

	if (const auto result = ::ledc_timer_config(&ledc_timer); result != ESP_OK)
	{
		co_return bzd::error::EspErr("ledc_timer_config", result);
	}

	if (const auto result = ::ledc_timer_resume(LEDC_MODE, number_); result != ESP_OK)
	{
		co_return bzd::error::EspErr("ledc_timer_resume", result);
	}

	co_return {};
}

bzd::Async<> LEDCTimer::shutdown() noexcept
{
	if (const auto result = ::ledc_timer_pause(LEDC_MODE, number_); result != ESP_OK)
	{
		co_return bzd::error::EspErr("ledc_timer_pause", result);
	}

	co_return {};
}

} // namespace bzd::components::esp32
