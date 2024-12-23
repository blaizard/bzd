#pragma once

#include "cc/bzd/algorithm/copy.hh"
#include "cc/bzd/container/optional.hh"
#include "cc/bzd/container/ring_buffer.hh"
#include "cc/bzd/core/async.hh"
#include "cc/bzd/core/channel.hh"
#include "cc/bzd/type_traits/container.hh"
#include "cc/bzd/type_traits/range.hh"
#include "cc/bzd/utility/ranges/views/drop.hh"
#include "cc/bzd/utility/ranges/views/reverse.hh"
#include "cc/bzd/utility/scope_guard.hh"

namespace bzd {

/// Create a buffered input channel.
/// It provide useful functionality for reading data from an input channel.
///
/// \tparam T The type of the channel.
/// \tparam capacity The maximal capacity of the buffer.
template <class T, Size bufferCapacity>
class IChannelBuffered : public bzd::IChannel<T>
{
public:
	class ReadScope : public ranges::Stream<bzd::Spans<const T, 3u>>
	{
	private:
		using Parent = ranges::Stream<bzd::Spans<const T, 3u>>;

	public:
		constexpr ReadScope(IChannelBuffered& ichannel, const bzd::Spans<const T, 3u> spans) noexcept :
			Parent{bzd::inPlace, bzd::move(spans)}, ichannel_{ichannel}
		{
		}

		ReadScope(const ReadScope&) = delete;
		ReadScope& operator=(const ReadScope&) = delete;
		constexpr ReadScope(ReadScope&& other) noexcept : Parent{static_cast<Parent&&>(other)}, ichannel_{other.ichannel_}
		{
			other.ichannel_.reset();
		}
		constexpr ReadScope& operator=(ReadScope&& other) noexcept
		{
			close();

			static_cast<Parent&>(*this) = static_cast<Parent&&>(other);
			ichannel_ = other.ichannel_;
			other.ichannel_.reset();

			return *this;
		}
		constexpr ~ReadScope() noexcept { close(); }

	private:
		constexpr void close() noexcept
		{
			if (ichannel_.hasValue())
			{
				const auto unconsumed = bzd::distance(this->it_, this->end());
				const auto left = this->range_.get().last(unconsumed);
				for (const auto& span : left.spans() | bzd::ranges::reverse())
				{
					ichannel_.valueMutable().buffer_.unconsume(span);
				}
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

private:
	using ReaderRangeBegin =
		iterator::InputOrOutputReference<typename bzd::Span<const T>::Iterator,
										 iterator::InputOrOutputReferencePolicies<typeTraits::IteratorCategory::input>>;
	using ReaderRangeEnd = typename bzd::Span<const T>::ConstIterator;
	using ReaderRange = ranges::SubRange<ReaderRangeBegin, ReaderRangeEnd>;

public:
	/// A generator to read from the input channel.
	///
	/// \note Upon destruction of the range, the data consumed will "unconsumed" from the buffer range,
	/// this feature allow zero copy when possible.
	bzd::Generator<ReaderRange> readerAsRange() noexcept
	{
		while (true)
		{
			auto data = buffer_.asSpanForReading();
			if (!data.empty())
			{
				buffer_.consume(data.size());
			}
			else
			{
				auto writeToBuffer = buffer_.asSpanForWriting();
				const auto maximumSize = writeToBuffer.size();
				data = co_await !in_.read(bzd::move(writeToBuffer));
				if (data.empty())
				{
					break; // no more data, exit.
				}
				bzd::assert::isTrue(data.size() <= maximumSize, "read too much data");
				buffer_.produce(data.size());
				buffer_.consume(data.size());
			}

			auto it = bzd::begin(data);
			const auto end = bzd::end(data);

			auto scope = bzd::ScopeGuard{[this, &it, &end]() {
				const auto left = bzd::Span<const T>{it, end};
				buffer_.unconsume(left);
			}};

			do
			{
				co_yield ReaderRange{ReaderRangeBegin{it}, end};
			} while (it != end);
		}
	}

	/// A generator to read from the input channel.
	///
	/// \note Upon destruction of the range, the data consumed will "unconsumed" from the buffer range,
	/// this feature allow zero copy when possible.
	bzd::Generator<const T&> reader() noexcept
	{
		auto generator = readerAsRange();
		auto it = co_await !generator.begin();
		while (it != generator.end())
		{
			auto itData = bzd::begin(*it);
			const auto endData = bzd::end(*it);

			while (itData != endData)
			{
				co_yield *itData;
				++itData;
			}

			co_await !++it;
		}
	}

	/// Read at least `count` bytes and return a Stream range.
	///
	/// \note Upon destruction of the range, the data consumed will be removed from the buffer ring.
	/*bzd::Async<ReadScope> read(const bzd::Size count) noexcept
	{
		// Span to hold freshly read buffer.
		auto buffer = buffer_.asSpans();
		bzd::Span<const T> dataRead{};

		// Read data.
		if (buffer.size() < count)
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
					co_return {};
				}
				// If there are enough data, exit the loop.
				else if ((buffer.size() + dataRead.size()) >= count)
				{
					break;
				}
				// Copy the buffer to the ring buffer if needed.
				else if (!dataRead.isSubSpan(writeToBuffer))
				{
					bzd::algorithm::copy(dataRead, writeToBuffer);
				}

				// Update the buffer
				buffer = buffer_.asSpans();
			}
		}

		bzd::Spans<const T, 3u> spans{bzd::inPlace, buffer[0], buffer[1], dataRead};
		buffer_.consume(spans.size());
		co_return ReadScope{*this, spans};
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

	/*
	void print()
	{
		const auto spans = buffer_.asSpans();
		print(spans);
	}
	template <class Spans>
	void print(const Spans& spans)
	{
		::std::cout << "[" << spans.size() << "] [";
		for (const auto c : spans)
		{
			::std::cout << c;
		}
		::std::cout << "]" << ::std::endl;
	}
	*/

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

			// To avoid unnecessary processing, only keep the first span and `minSize` of the second span.
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

template <Size bufferCapacity>
using IStreamBuffered = IChannelBuffered<bzd::Byte, bufferCapacity>;

} // namespace bzd
