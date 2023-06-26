#pragma once

#include "cc/bzd/container/ring_buffer.hh"
#include "cc/bzd/container/span.hh"
#include "cc/bzd/container/string_view.hh"
#include "cc/bzd/core/async.hh"
#include "cc/bzd/core/channel.hh"
#include "cc/bzd/platform/types.hh"
#include "cc/bzd/utility/to_underlying.hh"

namespace bzd::test {

enum class IChannelMode
{
	/// Copy the data into the buffer passed into argument. It iwll try to fill it as much as possible.
	none = 0,
	/// Simulate zero-copy by returning the internal buffer instead of the one passed into argument.
	zeroCopy = 1,
	/// Return only 1 element at a time, to simulate returning chunks of data.
	chunks = 2
};

constexpr IChannelMode operator|(const IChannelMode a, const IChannelMode b) noexcept
{
	return static_cast<IChannelMode>(bzd::toUnderlying(a) | bzd::toUnderlying(b));
}

constexpr bzd::Bool operator&(const IChannelMode a, const IChannelMode b) noexcept
{
	return static_cast<bzd::Bool>(bzd::toUnderlying(a) & bzd::toUnderlying(b));
}

/// Emulation of an input channel.
///
/// \tparam useInternalBuffer Return data from the internal buffer instead of the one passed into argument.
///                           This behavior is use to support zero copy.
template <class T, bzd::Size capacity, IChannelMode mode = IChannelMode::none>
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
		if (data.empty())
		{
			co_return bzd::error::Failure("The input buffer is empty."_csv);
		}

		auto spanForReading = buffer_.asSpanForReading();
		if constexpr (mode & IChannelMode::chunks)
		{
			spanForReading = spanForReading.first(1u);
		}

		if constexpr (mode & IChannelMode::zeroCopy)
		{
			const auto count = bzd::min(data.size(), spanForReading.size());
			buffer_.consume(count);
			co_return spanForReading.first(count);
		}
		else
		{
			bzd::Size index{0};
			for (const auto b : spanForReading)
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
	}

protected:
	bzd::RingBuffer<T, capacity> buffer_{};
};

template <bzd::Size capacity, IChannelMode mode = IChannelMode::none>
using IStream = IChannel<bzd::Byte, capacity, mode>;

} // namespace bzd::test
