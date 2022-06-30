#pragma once

#include "cc/bzd/container/ring_buffer.hh"
#include "cc/bzd/container/span.hh"
#include "cc/bzd/container/string_view.hh"
#include "cc/bzd/core/async.hh"
#include "cc/bzd/core/channel.hh"
#include "cc/bzd/platform/types.hh"

namespace bzd::test {

template <bzd::Size N>
class IStream : public bzd::IStream
{
protected:
	using Self = IStream;

public:
	Self& operator<<(const bzd::StringView data) noexcept
	{
		for (const auto c : data)
		{
			buffer_.pushBack(static_cast<bzd::Byte>(c));
		}
		return *this;
	}

	bzd::Async<bzd::Span<bzd::Byte>> read(const bzd::Span<bzd::Byte> data) noexcept override
	{
		if (buffer_.empty())
		{
			co_return bzd::Span<bzd::Byte>{};
		}
		bzd::Size index{0};
		for (const auto b : buffer_.asSpanForReading())
		{
			if (index >= data.size())
			{
				break;
			}
			data[index++] = b;
			buffer_.consume(1);
		}
		co_return data.subSpan(0, index);
	}

private:
	bzd::RingBuffer<bzd::Byte, N> buffer_{};
};

} // namespace bzd::test
