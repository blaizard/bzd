#pragma once

#include "bzd/core/channel.h"
#include "bzd/core/port.h"

namespace bzd { namespace io {
class DigitalOutput : public bzd::OChannel
{
public:
	DigitalOutput(const bzd::Port& port);

	// bzd::SizeType write(const bzd::UInt8Type& data) noexcept override;
	bzd::SizeType write(const bzd::Span<const bzd::UInt8Type>& data) noexcept override;

private:
	const bzd::Port& port_;
};
}} // namespace bzd::io
