#pragma once

#include "cc/libraries/io/digital/digital.hh"
#include "driver/gpio.hh"

namespace bzd::io::impl {
class DigitalOutputEsp32XtensaLx6 : public bzd::io::DigitalOutput
{
public:
	constexpr DigitalOutputEsp32XtensaLx6(const bzd::UInt8& pin) : bzd::io::DigitalOutput{}, pin_{static_cast<gpio_num_t>(pin)} {}

	bzd::Result<> connect() override;
	bzd::Result<Size> write(const bzd::Span<const bzd::UInt8>& data) noexcept;

private:
	const gpio_num_t pin_;
};
} // namespace bzd::io::impl
