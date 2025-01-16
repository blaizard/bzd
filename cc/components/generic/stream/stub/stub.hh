#pragma once

#include "cc/components/generic/stream/stub/interface.hh"

namespace bzd::components::generic::stream {
class Stub : public bzd::IOStream
{
public:
	template <class Context>
	constexpr explicit Stub(Context&) noexcept
	{
	}

	bzd::Async<> write(const bzd::Span<const bzd::Byte>) noexcept override { co_return {}; }

protected:
	bzd::Generator<bzd::Span<const bzd::Byte>> readerImpl(bzd::Span<bzd::Byte>) noexcept override { co_yield bzd::Span<const bzd::Byte>{}; }
};
} // namespace bzd::components::generic::stream
