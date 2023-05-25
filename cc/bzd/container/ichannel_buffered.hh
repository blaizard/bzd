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
	class ReaderScope : public range::Stream<typename bzd::Span<const T>::Iterator>
	{
	private:
		using Parent = range::Stream<typename bzd::Span<const T>::Iterator>;

	public:
		constexpr ReaderScope(IChannelBuffered& ichannel, const bzd::Span<const T> span, const bzd::Span<T> write = {}) noexcept :
			Parent{span.begin(), span.end()}, ichannel_{ichannel}, write_{write}
		{
		}

		ReaderScope(const ReaderScope&) = delete;
		ReaderScope& operator=(const ReaderScope&) = delete;
		constexpr ReaderScope(ReaderScope&& other) noexcept : Parent{bzd::move(other)}, ichannel_{other.ichannel_}, write_{other.write_}
		{
			other.ichannel_.reset();
		}
		constexpr ReaderScope& operator=(ReaderScope&& other) noexcept
		{
			close();

			static_cast<Parent&>(*this) = static_cast<Parent&&>(other);
			ichannel_ = other.ichannel_;
			write_ = other.write_;
			other.ichannel_.reset();

			return *this;
		}
		constexpr ~ReaderScope() noexcept { close(); }

	private:
		constexpr void close() noexcept
		{
			if (ichannel_.hasValue())
			{
				const auto size = bzd::distance(this->it_, this->end_);
				if (!bzd::Span<const T>(&(*this->it_), size).isSubSpan(write_))
				{
					bzd::algorithm::copy(*this, write_ | range::drop(write_.size() - size));
				}
				ichannel_.valueMutable().buffer_.unconsume(size);
			}
		}

	private:
		bzd::Optional<IChannelBuffered&> ichannel_;
		bzd::Span<T> write_;
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
	/// Uppon destruction of the range, the data consumed will be reflected in the buffer ring.
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
		co_return ReaderScope{*this, data, writeToBuffer.first(data.size())};
	}

	/*
		/// Read a minimum of `count` bytes and return a Stream range.
		///
		/// When destroyed, the stream range writes back the bytes that have not been
		/// consumed by the stream range into the buffer.
		bzd::Async<Scope> read(const bzd::Size minSize) noexcept
		{
			Scope scope{buffer_.asSpans()};

			for (Size index = 2u; index < 4u && scope.size() < minSize; ++index)
			{
				auto span = buffer_.asSpanForWriting();
				if (span.empty())
				{
					co_return bzd::error::Failure("Buffer of {} entries is full."_csv, buffer_.size());
				}
				const auto data = co_await !in_.readToBuffer(bzd::move(span));
				scope[index] = data;
			}

			co_return bzd::move(scope);
		}
	*/

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
