#pragma once

#include "cc/bzd/core/channel.hh"

namespace bzd::platform::generic::stream {
class Stub : public bzd::IOStream
{
public:
	bzd::Async<Size> write(const bzd::Span<const bzd::Byte> data) noexcept override { co_return data.size(); }

	bzd::Async<bzd::Span<bzd::Byte>> read(const bzd::Span<bzd::Byte>) noexcept override { co_return bzd::Span<bzd::Byte>{}; }
};
} // namespace bzd::platform::generic::stream
