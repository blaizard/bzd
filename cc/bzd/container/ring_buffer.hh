#pragma once

#include "cc/bzd/algorithm/copy.hh"
#include "cc/bzd/container/span.hh"
#include "cc/bzd/container/spans.hh"
#include "cc/bzd/container/storage/fixed.hh"
#include "cc/bzd/core/assert/minimal.hh"
#include "cc/bzd/platform/types.hh"
#include "cc/bzd/utility/in_place.hh"
#include "cc/bzd/utility/move.hh"

namespace bzd::impl {
/// Ring buffer implementation.
/// |-----|xxxxxxxxxxxx|--------|
/// 0    read         write  capacity
template <class T, class Storage>
class RingBuffer
{
public: // Traits.
	using Self = RingBuffer<T, Storage>;
	using StorageType = Storage;
	using ValueType = typename StorageType::ValueType;
	using ValueMutableType = typename StorageType::ValueMutableType;

public: // Constructors/assignments.
	RingBuffer() = default;

	RingBuffer(const Self&) = default;
	Self& operator=(const Self&) = default;
	RingBuffer(Self&&) = default;
	Self& operator=(Self&&) = default;
	~RingBuffer() = default;

public: // Size.
	[[nodiscard]] constexpr bzd::Bool empty() const noexcept { return size() == 0; }
	[[nodiscard]] constexpr bzd::Bool full() const noexcept { return size() >= capacity(); }
	[[nodiscard]] constexpr bzd::Size size() const noexcept { return (write_ - read_); }
	[[nodiscard]] constexpr bzd::Size capacity() const noexcept { return storage_.size(); }
	[[nodiscard]] constexpr bzd::Bool overrun() const noexcept { return overrun_; }

public: // Accessors.
	[[nodiscard]] constexpr auto& operator[](const Size index) noexcept { return at(index); }
	[[nodiscard]] constexpr auto& operator[](const Size index) const noexcept { return at(index); }
	[[nodiscard]] constexpr auto& at(const Size index) noexcept { return storage_.dataMutable()[(read_ + index) % capacity()]; }
	[[nodiscard]] constexpr auto& at(const Size index) const noexcept { return storage_.data()[(read_ + index) % capacity()]; }

	/// Get the spans container over the reading memory region.
	///
	/// \return The spans container.
	[[nodiscard]] constexpr auto asSpans() const noexcept
	{
		const auto start = read_ % capacity();
		const auto end = write_ % capacity();
		const auto secondSpan = (!empty() && (end <= start)) ? bzd::Span<ValueType>{storage_.data(), end} : bzd::Span<ValueType>{};

		return bzd::Spans<ValueType, 2u>{inPlace, asSpanForReading(), secondSpan};
	}

	/// Get the span of contiguous memory containing data starting from the tail.
	/// In other word, this represents the first half of contiguous data to be read.
	///
	/// \return A span containing the contiguous memory region.
	[[nodiscard]] constexpr auto asSpanForReading() const noexcept
	{
		if (empty())
		{
			return bzd::Span<ValueType>{};
		}
		const auto start = read_ % capacity();
		const auto end = write_ % capacity();
		return bzd::Span<ValueType>{&storage_.data()[start], (end > start) ? (end - start) : (capacity() - start)};
	}

	/// Get the span of contiguous memory containing free slots starting from the head.
	/// In other word, this represents the first half of contiguous free slots.
	///
	/// \return A span containing the contiguous memory region.
	[[nodiscard]] constexpr auto asSpanForWriting() noexcept
	{
		if (full())
		{
			return bzd::Span<ValueMutableType>{};
		}
		const auto start = write_ % capacity();
		const auto end = read_ % capacity();
		return bzd::Span<ValueMutableType>{&storage_.dataMutable()[start], (end > start) ? (end - start) : (capacity() - start)};
	}

public: // Modifiers.
	/// Consume the ring buffer from the tail.
	constexpr void consume(const bzd::Size n) noexcept
	{
		bzd::assert::isTrue(read_ + n <= write_, "Consumer reads passed written pointer.");
		read_ += n;
	}

	/// Assign the given span to the ring buffer. All previously available data are
	/// removed.
	constexpr void assign(const bzd::Span<const T> span) noexcept
	{
		// If the span is within the storage.
		if (span.data() >= storage_.data() && span.data() < &storage_.data()[storage_.size()])
		{
			read_ = (span.data() - storage_.data()) / sizeof(T);
			write_ = read_ + span.size();
		}
		else if (span.size() <= storage_.size())
		{
			algorithm::copy(span.begin(), span.end(), storage_.dataMutable());
			read_ = 0;
			write_ = span.size();
		}
		else
		{
			algorithm::copy(span.end() - storage_.size(), span.end(), storage_.dataMutable());
			read_ = 0;
			write_ = span.size();
			overrun_ = true;
		}
	}

	/// Produce new data in the ring buffer.
	constexpr void produce(const bzd::Size n) noexcept
	{
		write_ += n;
		if (size() > capacity())
		{
			read_ = write_ - capacity();
			overrun_ = true;
		}
	}

	constexpr void pushBack(const T& value) noexcept
	{
		storage_.dataMutable()[write_ % capacity()] = value;
		produce(1);
	}

	constexpr auto&& popFront() noexcept
	{
		auto& data = at(0);
		consume(1);
		return bzd::move(data);
	}

	constexpr void clear() noexcept
	{
		write_ = 0;
		read_ = 0;
		clearOverrun();
	}

	constexpr void clearOverrun() noexcept { overrun_ = false; }

private: // Variables.
	StorageType storage_{};
	bzd::Size write_{0};
	bzd::Size read_{0};
	bzd::Bool overrun_{false};
};
} // namespace bzd::impl

namespace bzd {
template <class T, Size capacity>
class RingBuffer : public impl::RingBuffer<T, impl::FixedStorage<T, capacity>>
{
	static_assert((capacity & (capacity - 1)) == 0, "The capacity of a RingBuffer must be a power of 2.");

protected:
	using Parent = impl::RingBuffer<T, impl::FixedStorage<T, capacity>>;
	using Self = RingBuffer<T, capacity>;
	using StorageType = typename Parent::StorageType;

public: // Constructor.
	RingBuffer() = default;
};
} // namespace bzd
