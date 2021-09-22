#pragma once

#include "cc/bzd/core/channel.h"

namespace bzd::platform::generic {
class StreamStub : public bzd::IOStream
{
public:
	bzd::Async<SizeType> write(const bzd::Span<const bzd::ByteType> data) noexcept override { co_return data.size(); }

	bzd::Async<SizeType> read(const bzd::Span<bzd::ByteType>) noexcept override { co_return 0; }
};
} // namespace bzd::platform::generic
