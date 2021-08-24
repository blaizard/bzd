#include "cc/libraries/io/digital/digital_esp32_xtensa_lx6.h"

namespace bzd::io::impl {

bzd::Result<> DigitalOutputEsp32XtensaLx6::connect()
{
	gpio_pad_select_gpio(pin_);
	gpio_set_direction(pin_, GPIO_MODE_OUTPUT);

	return bzd::nullresult;
}

bzd::Result<SizeType> DigitalOutputEsp32XtensaLx6::write(const bzd::Span<const bzd::UInt8Type>& data) noexcept
{
	if (data[0])
	{
		gpio_set_level(pin_, 1);
	}
	else
	{
		gpio_set_level(pin_, 0);
	}
	return 1;
}

} // namespace bzd::io::impl
