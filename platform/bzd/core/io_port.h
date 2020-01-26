#pragma once

#include "bzd/types.h"

namespace bzd {

class IOPort
{
public:
	IOPort(const bzd::UInt8Type port);

	void write(const bzd::BoolType level) noexcept;

	bzd::BoolType read() noexcept;
};

} // namespace bzd
