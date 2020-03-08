#pragma once

#include "libraries/io/i2c/i2c.h"

namespace bzd { namespace io { namespace impl {
class I2CEsp32XtensaLx6 : public bzd::io::I2C
{
public:
	struct Configuration : public bzd::io::I2C::Configuration
	{
		bzd::UInt8Type interface = 0;
	};

	constexpr I2CEsp32XtensaLx6(const Configuration& config)
			: bzd::io::I2C{}, config_{config}
	{
	}

	bzd::Expected<void> connect() override;

	bzd::Expected<SizeType> write(const bzd::Span<const bzd::UInt8Type>& data) noexcept override;
	bzd::Expected<SizeType> read(bzd::Span<bzd::UInt8Type>& data) noexcept override;

private:
	Configuration config_;
};
}}} // namespace bzd::io
