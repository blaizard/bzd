#pragma once

#include "bzd/types.h"

namespace bzd {

/**
 * A port is the end of a channel. It must have a direction or function (for special ports).
 */
class Port
{
public:
	constexpr Port(const bzd::UInt8Type port) : port_(port) {}

	constexpr bzd::UInt8Type get() const noexcept { return port_; }

private:
	const bzd::UInt8Type port_;
};

} // namespace bzd
