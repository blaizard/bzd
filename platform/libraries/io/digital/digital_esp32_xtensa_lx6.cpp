#include "bzd/io/port.h"
#include "driver/gpio.h"

#pragma once

#include "bzd/core/channel.h"

#include <iostream>

namespace bzd { namespace io {
class Stdout : public bzd::OChannel
{
public:
	bzd::SizeType write(const bzd::Span<const bzd::UInt8Type>& data) noexcept override
	{
		std::cout.write(reinterpret_cast<const char*>(data.data()), data.size());
		return data.size();
	}
};
}} // namespace bzd::io

namespace bzd { namespace io {

Port::Port(const bzd::UInt8Type port, const Port::Mode mode) : impl::Port(port)
{
	gpio_pad_select_gpio(port);
	switch (mode)
	{
	case Mode::INPUT:
		gpio_set_direction(static_cast<gpio_num_t>(port), GPIO_MODE_INPUT);
		break;
	case Mode::OUTPUT:
		gpio_set_direction(static_cast<gpio_num_t>(port), GPIO_MODE_OUTPUT);
		break;
	}
}

}}
