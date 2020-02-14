#pragma once

#include "driver/gpio.h"
#include "libraries/io/digital/digital.h"

namespace bzd { namespace io { namespace impl {
class DigitalOutputEsp32XtensaLx6 : public bzd::io::DigitalOutput
{
public:
	DigitalOutputEsp32XtensaLx6(const bzd::UInt8Type& pin);

	bzd::SizeType write(const bzd::Span<const bzd::UInt8Type>& data) noexcept override;

private:
	const gpio_num_t pin_;
};
}}} // namespace bzd::io
