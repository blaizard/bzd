#pragma once

#include "bzd/core/port.h"

namespace bzd { namespace io {
class PortEsp32XtensaLx6 : public bzd::Port
{
public:
	PortEsp32XtensaLx6(const bzd::UInt8Type number) : bzd::Port(number) {}
};
}} // namespace bzd::io
