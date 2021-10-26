#pragma once

#include "cc/bzd/core/channel.hh"

namespace bzd::platform::generic::stream {
class Stub : public bzd::IOStream
{
public:
	bzd::Async<SizeType> write(const bzd::Span<const bzd::ByteType> data) noexcept override { co_return data.size(); }

	bzd::Async<SizeType> read(const bzd::Span<bzd::ByteType>) noexcept override { co_return 0U; }
};
} // namespace bzd::platform::generic::stream
