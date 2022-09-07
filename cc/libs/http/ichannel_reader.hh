#pragma once

#include "cc/bzd/container/ring_buffer.hh"
#include "cc/bzd/core/async.hh"
#include "cc/bzd/core/channel.hh"

namespace bzd {

/// Create an input channel reader implementation.
/// It provide usefull functionality for reading data from an input channel.
///
/// \tparam capacity The maximal capacity of the buffer.
template <class T, Size capacity>
class IChannelReader : public bzd::IChannel<T>
{
public:
	constexpr explicit IChannelReader(bzd::IChannel<T>& in) noexcept : in_{in} {}

	/// Unconditional read from the input channel.
	///
	/// Note this function might return a buffer that is different from the one passed into argument.
	bzd::Async<bzd::Span<const T>> read(bzd::Span<T>&& data) noexcept override
	{
		const auto readFromBuffer = buffer_.asSpanForReading();
		if (readFromBuffer.empty())
		{
			co_return co_await in_.read(bzd::move(data));
		}
		const auto count = bzd::min(data.size(), readFromBuffer.size());
		buffer_.consume(count);
		co_return readFromBuffer.first(count);
	}

	/// Read data until a specific value is found.
	bzd::Async<bzd::Span<const T>> readUntil(const T& value) noexcept
	{
		auto readFromBuffer = buffer_.asSpanForReading();
		if (readFromBuffer.empty())
		{
			readFromBuffer = co_await !produce();
		}
		const auto position = bzd::algorithm::find(readFromBuffer, value);
		if (position == readFromBuffer.end())
		{
			co_return readFromBuffer;
		}
		co_return readFromBuffer.first(position);
	}

	/*    /// Read data until a specific sequence is found.
		template <class Iterator>
		requires concepts::forwardIterator<Iterator>
		bzd::Async<bzd::Span<T>> readUntil(Iterator first, Iterator last) noexcept
		{
			const auto readFromBuffer = buffer_.asSpanForReading();
		}
	*/

private:
	/// Produces new data in the ring buffer read from the input stream
	/// and return what has been read.
	[[nodiscard]] bzd::Async<bzd::Span<bzd::Byte>> produce() noexcept
	{
		auto span = buffer_.asSpanForWriting();
		if (span.empty())
		{
			co_return bzd::error::Failure("Buffer of {} entries is full."_csv, buffer_.size());
		}
		auto result = co_await !in_.readToBuffer(span);
		buffer_.produce(result.size());

		co_return result;
	}

private: // Variables.
	bzd::IChannel<T>& in_;
	bzd::RingBuffer<T, capacity> buffer_;
};

template <Size capacity>
using IStreamReader = IChannelReader<bzd::Byte, capacity>;

} // namespace bzd
