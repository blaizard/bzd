#pragma once

#include "cc/bzd/container/ring_buffer.hh"
#include "cc/bzd/container/span.hh"
#include "cc/bzd/container/string_view.hh"
#include "cc/bzd/core/async.hh"
#include "cc/bzd/core/channel.hh"
#include "cc/bzd/platform/types.hh"

namespace bzd::test {

template <class T, bzd::Size capacity>
class IChannel : public bzd::IChannel<T>
{
protected:
	using Self = IChannel;

public:
	/// Fill the input buffer from a string.
	Self& operator<<(const bzd::StringView data) noexcept
	{
		for (const auto c : data)
		{
			buffer_.pushBack(static_cast<T>(c));
		}
		return *this;
	}

	bzd::Async<bzd::Span<const T>> read(bzd::Span<T>&& data) noexcept override
	{
		if (buffer_.empty())
		{
			co_return bzd::error::Eof("No more data available."_csv);
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
		co_return data.first(index);
	}

private:
	bzd::RingBuffer<T, capacity> buffer_{};
};

template <bzd::Size capacity>
using IStream = IChannel<bzd::Byte, capacity>;

} // namespace bzd::test
