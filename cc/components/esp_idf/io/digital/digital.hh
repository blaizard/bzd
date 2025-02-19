#pragma once

#include "cc/components/esp_idf/io/digital/interface.hh"

#include <driver/gpio.h>

namespace bzd::components::esp32::io {

template <class Context>
class Digital
{
public:
	constexpr Digital(Context& context) noexcept : context_{context} {}

	bzd::Async<> init() noexcept
	{
		switch (context_.config.mode)
		{
		case DigitalMode::input:
			::gpio_set_direction(pin_, GPIO_MODE_INPUT);
			break;
		case DigitalMode::output:
			::gpio_set_direction(pin_, GPIO_MODE_OUTPUT);
			break;
		case DigitalMode::ouput_open_drain:
			::gpio_set_direction(pin_, GPIO_MODE_OUTPUT_OD);
			break;
		case DigitalMode::input_ouput_open_drain:
			::gpio_set_direction(pin_, GPIO_MODE_INPUT_OUTPUT_OD);
			break;
		case DigitalMode::input_output:
			::gpio_set_direction(pin_, GPIO_MODE_INPUT_OUTPUT);
			break;
		}
		co_return {};
	}

private:
	Context& context_;
	::gpio_num_t pin_{context_.config.pin};
};

} // namespace bzd::components::esp32::io
