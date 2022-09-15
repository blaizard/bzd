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


		/// Other solution, mixing both
		auto readUntil(const T& value) noexcept
		{
			return [](Function<bzd::Span<T>(void)>) -> bzd::Generator<bzd::Span<const T>> {
			};
		}

		auto factory = readUntil(...);
		for (auto x : factory([&]() { return hello.subSpan(); }))
		{
			x
		}

		/// Another
		struct Context
		{
			Span read(Size)
			{
			}
		};

		readContext = makeReadContext(myString);

		bzd::Generator<bzd::Span<const T>> readUntil(ContextReader context, const T& value) noexcept
		{
			data = context.allocate();
			readFromBuffer = co_await in_.read(data);
		}

		bzd::Generator<bzd::Span<const T>> readCount(ContextReader context, const Size count) noexcept
		{
			bzd::ignore = co_await !context.read(in_, 12);
		}

		/// Or maybe the best is to:
		/// 1. return bzd::Generator<bzd::Span<const T>>, because most of the functions need to write to the internal buffer anyway,
		/// 2. for the few (only thinking about readCount) provide an overload that can write directly into a container.

		template <class T>
		class IFiniteChannel : public bzd::IChannel<T>
		{
		public:
			virtual Bool empty() noexcept = 0;
		};

		/// Advantage:
		/// - Can pass a buffer to be written to.
		/// - Compatible with IChannel.
		/// - Implementation can be splitted in different files.
		///
		/// - Function is not stateful
		///        -> but class is.
		/// - Function more complex and less efficient as it needs to start from the top all the time.
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

	/// Read data until a specific value is found but does not return the value.
	auto readUntil(const T value) noexcept
	{
		return readUntilHelper(
			[value](const auto range) noexcept -> bzd::Size {
				if (const auto it = bzd::algorithm::find(range, value); it != range.end())
				{
					return bzd::distance(range.begin(), it);
				}
				return bzd::npos;
			},
			1u);
	}

	/// Read data until a specific sequence is found but does not include the sequence.
	auto readUntil(const bzd::Span<const T> value) noexcept
	{
		return readUntilHelper(
			[value](const auto range) noexcept -> bzd::Size {
				if (const auto it = bzd::algorithm::search(range, value); it != range.end())
				{
					return bzd::distance(range.begin(), it);
				}
				return bzd::npos;
			},
			value.size());
	}

private:
	/// Read data until a specific value is found.
	template <class Callback>
	bzd::Generator<bzd::Span<const T>> readUntilHelper(Callback callback, const Size minSize) noexcept
	{
		while (true)
		{
			// Fill the buffer if there is not enough data.
			if (buffer_.size() < minSize)
			{
				co_await !readToBuffer();
			}

			// Get the data and ensure it is enough.
			auto readFromBuffer = buffer_.asSpans();
			if (readFromBuffer.size() < minSize)
			{
				continue;
			}

			// To avoid unecessary processing, only keep the first span and `minSize` of the second span.
			// This will avoid having the callback processing extra data that will be re-processed in the second call.
			const auto [firstSpan, secondSpan] = readFromBuffer.spans();
			const bzd::Spans<const T, 2u> spans{bzd::inPlace,
												firstSpan,
												(secondSpan.size() > minSize) ? secondSpan.first(minSize) : secondSpan};

			// Call the callback and return the full or partial first span only.
			const auto position = callback(spans);
			if (position == bzd::npos)
			{
				buffer_.consume(firstSpan.size());
				co_yield firstSpan;
			}
			else
			{
				const auto updatedPosition = bzd::min(position, firstSpan.size());
				buffer_.consume(updatedPosition);
				co_yield firstSpan.first(updatedPosition);
				break;
			}
		}
	}

	/// Read new data from the input channel and return it.
	/// This action does not produce data to the ring buffer.
	[[nodiscard]] bzd::Async<bzd::Span<const T>> read() noexcept
	{
		auto span = buffer_.asSpanForWriting();
		if (span.empty())
		{
			co_return bzd::error::Failure("Buffer of {} entries is full."_csv, buffer_.size());
		}
		co_return co_await !in_.read(bzd::move(span));
	}

	/// Read new data from the input channel and return it.
	[[nodiscard]] bzd::Async<> readToBuffer() noexcept
	{
		auto span = buffer_.asSpanForWriting();
		if (span.empty())
		{
			co_return bzd::error::Failure("Buffer of {} entries is full."_csv, buffer_.size());
		}
		const auto data = co_await !in_.readToBuffer(bzd::move(span));
		buffer_.produce(data.size());
		co_return {};
	}

private: // Variables.
	bzd::IChannel<T>& in_;
	bzd::RingBuffer<T, capacity> buffer_;
};

template <class T, class Value>
requires requires(T&& t, Value&& v)
{
	t += v;
}
bzd::Async<T> make(bzd::Generator<Value>&& generator) noexcept
{
	T container;
	co_await !bzd::async::forEach(bzd::move(generator), [&container](const Value& value) {
		for (auto& a : value) container += static_cast<char>(a);
	});
	co_return container;
}

} // namespace bzd
