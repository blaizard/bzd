#pragma once

#include "bzd/core/channel.h"

namespace bzd { namespace io {
class PCA9685
{
public:
	constexpr PCA9685(bzd::IOChannel& i2c, const bzd::UInt8Type address) {}

	/**
	 * Ratio from 0 to 4096
	 */
	constexpr void setDutyCycle(const bzd::UInt16Type ratio) {}

private:
	bzd::IOChannel& i2c_;
	const bzd::UInt8Type address_ = 0;
};
}} // namespace bzd::io
