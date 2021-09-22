#pragma once

#include "cc/bzd/core/channel.h"

namespace bzd::platform::generic {
class ChannelStub : public bzd::IOChannel
{
public:
	bzd::Async<SizeType> write(const bzd::Span<const bzd::ByteType> data) noexcept override { co_return data.size(); }

	bzd::Async<SizeType> read(const bzd::Span<bzd::ByteType>) noexcept override { co_return 0; }
};
} // namespace bzd::platform::generic
