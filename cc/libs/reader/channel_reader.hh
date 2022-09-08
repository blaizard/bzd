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
class ChannelReader : public bzd::IChannel<T>
{
public:
	constexpr explicit ChannelReader(bzd::IChannel<T>& in) noexcept : in_{in} {}

	/// Unconditional read from the input channel.
	///
	/// Note this function might return a buffer that is different from the one passed into argument.
	bzd::Async<bzd::Span<const T>> read(bzd::Span<T>&& data) noexcept final
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
/*
	/// Problem with this solution:
	/// - Cannot use an input buffer to write data directly to it. Need to copy first to internal buffer.
	///       -> Wrong concern because data cannot be written to the output buffer directly as it needs to be read first.
	///              -> /!\ This is not always true for other type of functions, for example readNumberOfBytes(...).
	///       -> produce() can be changed to not produce data to the ring (so use .read instead of .readToBuffer).
	/// - Not compatible with IChannel<T>
	///       -> 
	///
	/// Advantages:
	/// - Simple to use.
	/// - The function is stateful, making it easier to write the logic.
	bzd::Generator<bzd::Span<const T>> readUntil(const T& value) noexcept
	{
		auto readFromBuffer = buffer_.asSpanForReading();
		const Size position;
		while (position = bzd::algorithm::find(readFromBuffer, value); position == readFromBuffer.end())
		{
			buffer_.consume(readFromBuffer.size());
			co_yield readFromBuffer;
			readFromBuffer = co_await !produce();
		}
		buffer_.consume(position);
		co_yield readFromBuffer.first(position);
	}


template <class T>
class IFiniteChannel : public bzd::IChannel<T>
{
public:
	virtual Bool empty() noexcept = 0;
};

	/// Advantage:
	/// - Can pass a buffer to be written to.
	/// - Compatible with IChannel.
	///
	/// - Function is not stateful
	///        -> but class is.
	IFiniteChannel readUntil(const T& value, filters...)
	{
	}

	channel = readUntil(const T& value, filters...);
	while (!channel.empty())
	{
		buffer = co_await channel.read(std::move(buffer));
	}

	for (const auto span : readUntil(...))
	{
	}
*/

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

} // namespace bzd
