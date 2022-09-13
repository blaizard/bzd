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
	bzd::Generator<bzd::Span<const T>> readUntil(const T value) noexcept
	{
		return readUntilHelper([value](const bzd::Span<const T> span) -> bzd::Size {
			if (const auto it = bzd::algorithm::find(span, value); it != span.end())
			{
				return bzd::distance(span.begin(), it);
			}
			return bzd::npos;
		});
	}

	/// Read data until a specific sequence is found but does not include the sequence.
	bzd::Generator<bzd::Span<const T>> readUntil(const bzd::Span<const T> value) noexcept
	{
		return readUntilHelper([value](const bzd::Span<const T> span) -> bzd::Size {
			if (const auto it = bzd::algorithm::search(span, value); it != span.end())
			{
				return bzd::distance(span.begin(), it);
			}
			return bzd::npos;
		});
	}

private:
	/// Read data until a specific value is found.
	template <class Callback>
	bzd::Generator<bzd::Span<const T>> readUntilHelper(Callback callback) noexcept
	{
		while (true)
		{
			if (auto readFromBuffer = buffer_.asSpanForReading(); !readFromBuffer.empty())
			{
				const auto position = callback(readFromBuffer);
				if (position == bzd::npos)
				{
					buffer_.consume(readFromBuffer.size());
					co_yield readFromBuffer;
				}
				else
				{
					buffer_.consume(position);
					co_yield readFromBuffer.first(position);
					break;
				}
			}
			else if (auto readFromBuffer = co_await !read(); !readFromBuffer.empty())
			{
				const auto position = callback(readFromBuffer);
				if (position == bzd::npos)
				{
					co_yield readFromBuffer;
				}
				else
				{
					auto left = readFromBuffer.subSpan(position);
					buffer_.assign(left);
					co_yield readFromBuffer.first(position);
					break;
				}
			}
			else
			{
				co_await bzd::async::yield();
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

private: // Variables.
	bzd::IChannel<T>& in_;
	bzd::RingBuffer<T, capacity> buffer_;
};

} // namespace bzd
