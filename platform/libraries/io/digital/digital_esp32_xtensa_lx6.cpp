#include "driver/gpio.h"

#include "libraries/io/digital/digital.h"
#include "libraries/io/port/port_esp32_xtensa_lx6.h"

namespace bzd { namespace io {

DigitalOutput::DigitalOutput(const bzd::Port& port) : bzd::OChannel(), port_(port)
{
	gpio_pad_select_gpio(port.get());
	gpio_set_direction(static_cast<gpio_num_t>(port.get()), GPIO_MODE_OUTPUT);
}

bzd::SizeType DigitalOutput::write(const bzd::Span<const bzd::UInt8Type>& data) noexcept
{
	const auto& targetPort = reinterpret_cast<const bzd::io::PortEsp32XtensaLx6&>(port_);

	if (data[0])
	{
		gpio_set_level(targetPort.getGpio(), 1);
	}
	else
	{
		gpio_set_level(targetPort.getGpio(), 0);
	}
	return 1;
}
/*
bzd::SizeType DigitalOutput::write(const bzd::UInt8Type& data) noexcept
{
}*/


}} // namespace bzd::io
