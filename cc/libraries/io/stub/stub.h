#pragma once

#include "cc/bzd/core/channel.h"

namespace bzd::io {
class Stub : public bzd::OChannel
{
public:
	bzd::Result<SizeType> write(const bzd::Span<const bzd::UInt8Type>& data) noexcept override { return data.size(); }
};
} // namespace bzd::io
