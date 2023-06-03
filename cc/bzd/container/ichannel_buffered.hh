#pragma once

#include "cc/bzd/algorithm/copy.hh"
#include "cc/bzd/container/optional.hh"
#include "cc/bzd/container/range/views/drop.hh"
#include "cc/bzd/container/ring_buffer.hh"
#include "cc/bzd/core/async.hh"
#include "cc/bzd/core/channel.hh"
#include "cc/bzd/type_traits/container.hh"
#include "cc/bzd/type_traits/range.hh"

namespace bzd {

/// Create a buffered input channel.
/// It provide usefull functionality for reading data from an input channel.
///
/// \tparam T The type of the channel.
/// \tparam capacity The maximal capacity of the buffer.
template <class T, Size bufferCapacity>
class IChannelBuffered : public bzd::IChannel<T>
{
public:
	/// Reader for data gather either from the input channel or the internal buffer.
	///
	/// Uppon destruction of the range, the data consumed will be reflected in the buffer ring.
	class ReaderScope : public range::Stream<bzd::Span<const T>>
	{
	private:
		using Parent = range::Stream<bzd::Span<const T>>;

	public:
		constexpr ReaderScope(IChannelBuffered& ichannel, const bzd::Span<const T>& span) noexcept :
			Parent{bzd::inPlace, bzd::move(span)}, ichannel_{ichannel}
		{
		}

		ReaderScope(const ReaderScope&) = delete;
		ReaderScope& operator=(const ReaderScope&) = delete;
		constexpr ReaderScope(ReaderScope&& other) noexcept : Parent{static_cast<Parent&&>(other)}, ichannel_{other.ichannel_}
		{
			other.ichannel_.reset();
		}
		constexpr ReaderScope& operator=(ReaderScope&& other) noexcept
		{
			close();

			static_cast<Parent&>(*this) = static_cast<Parent&&>(other);
			ichannel_ = other.ichannel_;
			other.ichannel_.reset();

			return *this;
		}
		constexpr ~ReaderScope() noexcept { close(); }

	private:
		constexpr void close() noexcept
		{
			if (ichannel_.hasValue())
			{
				const auto left = bzd::Span<const T>{this->it_, this->end()};
				ichannel_.valueMutable().buffer_.unconsume(left);
			}
		}

	private:
		bzd::Optional<IChannelBuffered&> ichannel_;
	};

public:
	constexpr explicit IChannelBuffered(bzd::IChannel<T>& in) noexcept : in_{in} {}

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

	/// Read from the input channel as a ReaderScope.
	///
	/// \note Uppon destruction of the range, the data consumed will be removed from the buffer ring.
	bzd::Async<ReaderScope> reader() noexcept
	{
		const auto readFromBuffer = buffer_.asSpanForReading();
		if (!readFromBuffer.empty())
		{
			buffer_.consume(readFromBuffer.size());
			co_return ReaderScope{*this, readFromBuffer};
		}
		auto writeToBuffer = buffer_.asSpanForWriting();
		const auto data = co_await !in_.read(bzd::move(writeToBuffer));
		buffer_.produce(data.size());
		buffer_.consume(data.size());
		co_return ReaderScope{*this, data};
	}

	/// Read at least `count` bytes and return a Stream range.
	///
	/// \note Uppon destruction of the range, the data consumed will be removed from the buffer ring.
	/*bzd::Async<ReaderScope<bzd::Spans<const T, 3u>, true>> read(const bzd::Size count) noexcept
	{
		using Spans = bzd::Spans<const T, 3u>;

		// Span to hold freshly read buffer.
		bzd::Span<const T> dataRead{};

		// Read data.
		if (buffer_.size() < count)
		{
			while (true)
			{
				auto writeToBuffer = buffer_.asSpanForWriting();
				// If there is no space for writing, it means that the ring buffer is too small to contain
				// the data.
				if (writeToBuffer.empty())
				{
					co_return bzd::error::Failure{"RingBuffer<{}> vs {}"_csv, bufferCapacity, count};
				}

				dataRead = co_await !in_.read(bzd::move(writeToBuffer));
				buffer_.produce(dataRead.size());

				// If there is no data, return an end of buffer failure.
				// This should be covered by the IChannel already.
				if (dataRead.empty())
				{
					co_return bzd::error::Eof{};
				}
				// If there are enough data, exit the loop.
				else if ((buffer_.size() + dataRead.size()) >= count)
				{
					break;
				}
				// Copy the buffer to the ring buffer if needed.
				else if (!dataRead.isSubSpan(writeToBuffer))
				{
					bzd::algorithm::copy(dataRead, writeToBuffer);
				}
			}
		}

		//const auto spans = buffer_.asSpans();
		//co_return ReaderScope<Spans, true>{Spans{bzd::inPlace, spans[0], spans[1], dataRead}};
		//co_return ReaderScope<Spans, true>{Spans{bzd::inPlace, dataRead}};
		Spans spans{bzd::inPlace, dataRead};
		//spans[2] = dataRead;
		co_return ReaderScope<Spans, true>{bzd::move(spans)};
	}*/

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

public:
	[[nodiscard]] constexpr bzd::Bool empty() const noexcept { return size() == 0; }
	[[nodiscard]] constexpr bzd::Bool full() const noexcept { return size() >= capacity(); }
	[[nodiscard]] constexpr bzd::Size size() const noexcept { return buffer_.size(); }
	[[nodiscard]] constexpr bzd::Size capacity() const noexcept { return buffer_.capacity(); }

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
	bzd::RingBuffer<T, bufferCapacity> buffer_;
};

} // namespace bzd
