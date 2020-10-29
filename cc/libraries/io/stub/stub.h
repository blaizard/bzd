#pragma once

#include "bzd/core/channel.h"

namespace bzd { namespace io {
class Stub : public bzd::OChannel
{
public:
	bzd::Result<SizeType> write(const bzd::Span<const bzd::UInt8Type>& data) noexcept override { return data.size(); }
};
}} // namespace bzd::io
