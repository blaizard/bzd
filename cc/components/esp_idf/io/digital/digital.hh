#pragma once

#include "cc/components/esp_idf/error.hh"
#include "cc/components/esp_idf/io/digital/interface.hh"

#include <driver/gpio.h>

namespace bzd::components::esp32::io {

template <class Context>
class Digital : public bzd::IO
{
public:
	constexpr Digital(Context& context) noexcept : context_{context} {}

	bzd::Async<> init() noexcept
	{
		co_await !reset();
		co_await !setMode(context_.config.mode);

		co_return {};
	}

	bzd::Async<> shutdown() noexcept
	{
		co_await !reset();
		co_return {};
	}

	bzd::Async<> set() noexcept override { co_return {}; }

	bzd::Async<> clear() noexcept override { co_return {}; }

	bzd::Async<> run() noexcept { co_return {}; }

private:
	bzd::Async<> reset() noexcept
	{
		if (const auto result = ::gpio_reset_pin(pin_); result != ESP_OK)
		{
			co_return bzd::error::EspErr("gpio_reset_pin", result);
		}
		co_return {};
	}

	bzd::Async<> setMode(const DigitalMode mode) noexcept
	{
		::gpio_mode_t gpioMode{};
		switch (mode)
		{
		case DigitalMode::input:
			gpioMode = GPIO_MODE_INPUT;
			break;
		case DigitalMode::output:
			gpioMode = GPIO_MODE_OUTPUT;
			break;
		case DigitalMode::ouputOpenDrain:
			gpioMode = GPIO_MODE_OUTPUT_OD;
			break;
		case DigitalMode::inputOuputOpenDrain:
			gpioMode = GPIO_MODE_INPUT_OUTPUT_OD;
			break;
		case DigitalMode::inputOutput:
			gpioMode = GPIO_MODE_INPUT_OUTPUT;
			break;
		}
		if (const auto result = ::gpio_set_direction(pin_, gpioMode); result != ESP_OK)
		{
			co_return bzd::error::EspErr("gpio_set_direction", result);
		}
		co_return {};
	}

private:
	Context& context_;
	::gpio_num_t pin_{static_cast<::gpio_num_t>(context_.config.pin)};
};

} // namespace bzd::components::esp32::io
