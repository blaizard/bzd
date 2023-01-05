#pragma once

#include "cc/bzd/core/channel.hh"

namespace bzd::platform::generic::stream {
class Stub : public bzd::IOStream
{
public:
	template <class Config>
	constexpr explicit Stub(const Config&) noexcept
	{
	}

	bzd::Async<> write(const bzd::Span<const bzd::Byte>) noexcept override { co_return {}; }

	bzd::Async<bzd::Span<const bzd::Byte>> read(bzd::Span<bzd::Byte>&&) noexcept override { co_return bzd::Span<const bzd::Byte>{}; }
};
} // namespace bzd::platform::generic::stream
