#pragma once

#include "bzd/core/port.h"
#include "driver/gpio.h"

namespace bzd { namespace io {
class PortEsp32XtensaLx6 : public bzd::Port
{
public:
	constexpr PortEsp32XtensaLx6(const bzd::UInt8Type number) : bzd::Port(number), gpio_(static_cast<gpio_num_t>(number)) {}

	constexpr gpio_num_t getGpio() const noexcept { return gpio_; }

private:
	gpio_num_t gpio_;
};
}} // namespace bzd::io
