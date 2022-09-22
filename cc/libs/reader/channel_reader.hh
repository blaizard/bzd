#pragma once

#include "cc/bzd/container/ring_buffer.hh"
#include "cc/bzd/core/async.hh"
#include "cc/bzd/core/channel.hh"
#include "cc/bzd/type_traits/container.hh"

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

	/// Read data until a specific value is found.
	auto readUntil(const T value, const Bool include = false) noexcept
	{
		return readUntilHelper(
			[value, include](const auto range) noexcept -> bzd::Size {
				if (const auto it = bzd::algorithm::find(range, value); it != range.end())
				{
					return bzd::distance(range.begin(), it) + ((include) ? 1u : 0u);
				}
				return bzd::npos;
			},
			1u);
	}

	/// Read data until a specific sequence is found.
	auto readUntil(const bzd::Span<const T> value, const Bool include = false) noexcept
	{
		return readUntilHelper(
			[value, include](const auto range) noexcept -> bzd::Size {
				if (const auto it = bzd::algorithm::search(range, value); it != range.end())
				{
					return bzd::distance(range.begin(), it) + ((include) ? value.size() : 0u);
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
requires concepts::containerAppendable<T, Value> bzd::Async<T> make(bzd::Generator<Value>&& generator)
noexcept
{
	T container;
	co_await !bzd::async::forEach(bzd::move(generator), [&container](const Value& value) { container.append(value); });
	co_return container;
}

} // namespace bzd
