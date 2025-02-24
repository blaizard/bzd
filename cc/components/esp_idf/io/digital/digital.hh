#pragma once

#include "cc/components/esp_idf/error.hh"
#include "cc/components/esp_idf/io/digital/interface.hh"

#include <driver/gpio.h>

namespace bzd::components::esp32::io::digital {

template <class Context>
class Output : public bzd::io::digital::Output<Output<Context>>
{
public:
	constexpr Output(Context& context) noexcept : context_{context} {}

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

	bzd::Async<> run() noexcept
	{
		while (true)
		{
			const auto data = co_await !context_.io.out.get();
			if (const auto result = ::gpio_set_level(pin_, data.value()); result != ESP_OK)
			{
				co_return bzd::error::EspErr("gpio_set_level", result);
			}
		}
		co_return {};
	}

private:
	bzd::Async<> reset() noexcept
	{
		if (const auto result = ::gpio_reset_pin(pin_); result != ESP_OK)
		{
			co_return bzd::error::EspErr("gpio_reset_pin", result);
		}
		co_return {};
	}

	bzd::Async<> setMode(const OutputMode mode) noexcept
	{
		::gpio_mode_t gpioMode{};
		switch (mode)
		{
		case OutputMode::normal:
			gpioMode = GPIO_MODE_OUTPUT;
			break;
		case OutputMode::openDrain:
			gpioMode = GPIO_MODE_OUTPUT_OD;
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

} // namespace bzd::components::esp32::io::digital
