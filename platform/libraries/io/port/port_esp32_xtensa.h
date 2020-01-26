#pragma once

#include "bzd/core/io_port.h"
#include "driver/gpio.h"

namespace bzd { namespace io {
class IOPort : public bzd::IOPort
{
public:
	void write(const bzd::BoolType& /*data*/) noexcept
	{
	}

	bzd::BoolType read() noexcept
	{
		return true;
	}
};
}} // namespace bzd::io
