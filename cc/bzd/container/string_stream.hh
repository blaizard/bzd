#pragma once

#include "cc/bzd/container/string.hh"
#include "cc/bzd/container/string_view.hh"
#include "cc/bzd/core/channel.hh"

namespace bzd::impl {
class StringStream : public bzd::OStream
{
public:
	constexpr StringStream(bzd::interface::String& str) : string_(str) {}

	bzd::Async<> write(const bzd::Span<const bzd::Byte> data) noexcept override
	{
		string_.pushBack(data);
		co_return {};
	}

	constexpr const bzd::interface::String& str() const noexcept { return string_; }

	constexpr bzd::interface::String& str() noexcept { return string_; }

protected:
	bzd::interface::String& string_;
};
} // namespace bzd::impl

namespace bzd::interface {
using StringStream = bzd::impl::StringStream;
}

namespace bzd {
template <Size N>
class StringStream : public bzd::interface::StringStream
{
public:
	constexpr StringStream() : bzd::interface::StringStream(container_), container_() {}

private:
	bzd::String<N> container_;
};

} // namespace bzd
