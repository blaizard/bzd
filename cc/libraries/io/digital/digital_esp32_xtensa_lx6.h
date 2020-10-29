#pragma once

#include "driver/gpio.h"
#include "libraries/io/digital/digital.h"

namespace bzd { namespace io { namespace impl {
class DigitalOutputEsp32XtensaLx6 : public bzd::io::DigitalOutput
{
public:
	constexpr DigitalOutputEsp32XtensaLx6(const bzd::UInt8Type& pin) : bzd::io::DigitalOutput{}, pin_{static_cast<gpio_num_t>(pin)} {}

	bzd::Result<void> connect() override;
	bzd::Result<SizeType> write(const bzd::Span<const bzd::UInt8Type>& data) noexcept override;

private:
	const gpio_num_t pin_;
};
}}} // namespace bzd::io::impl
