#include "bzd/core/io_port.h"

namespace bzd {

IOPort::IOPort(const bzd::UInt8Type /*port*/)
{
}

void IOPort::write(const bzd::BoolType /*data*/) noexcept
{
}

bzd::BoolType IOPort::read() noexcept
{
	return true;
}

} // namespace bzd::io
