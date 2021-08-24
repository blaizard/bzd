#pragma once

#include "cc/libraries/io/i2c/i2c.h"

namespace bzd::io::impl {
class I2CEsp32XtensaLx6 : public bzd::io::I2C
{
public:
	struct Configuration : public bzd::io::I2C::Configuration
	{
		bzd::UInt8Type interface = 0;
	};

	constexpr I2CEsp32XtensaLx6(const Configuration& config) : bzd::io::I2C{}, config_{config} {}

	bzd::Result<> connect() override;

	bzd::Result<SizeType> write(const bzd::Span<const bzd::UInt8Type>& data) noexcept;
	bzd::Result<SizeType> read(bzd::Span<bzd::UInt8Type>& data) noexcept;

private:
	Configuration config_;
};
} // namespace bzd::io::impl
