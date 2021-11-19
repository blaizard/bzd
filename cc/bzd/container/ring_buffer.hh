#pragma once

#include "cc/bzd/container/span.hh"
#include "cc/bzd/container/storage/fixed.hh"
#include "cc/bzd/core/assert/minimal.hh"
#include "cc/bzd/platform/types.hh"
#include "cc/bzd/utility/move.hh"
#include "cc/bzd/container/spans.hh"
#include "cc/bzd/utility/in_place.hh"

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
	using DataType = typename StorageType::DataType;
	using DataMutableType = typename StorageType::DataMutableType;

public: // Constructors/assignments.
	constexpr RingBuffer() noexcept = default;

	// Cannot be copied nor moved, the ring buffer should be passed by reference instead.
	constexpr RingBuffer(const Self&) noexcept = delete;
	constexpr Self& operator=(const Self&) noexcept = delete;
	constexpr RingBuffer(Self&&) noexcept = delete;
	constexpr Self& operator=(Self&&) noexcept = delete;

public: // Size.
	[[nodiscard]] constexpr bzd::BoolType empty() const noexcept { return size() == 0; }
	[[nodiscard]] constexpr bzd::BoolType full() const noexcept { return size() >= capacity(); }
	[[nodiscard]] constexpr bzd::SizeType size() const noexcept { return (write_ - read_); }
	[[nodiscard]] constexpr bzd::SizeType capacity() const noexcept { return storage_.size(); }
	[[nodiscard]] constexpr bzd::BoolType overrun() const noexcept { return overrun_; }

public: // Accessors.
	[[nodiscard]] constexpr auto& operator[](const SizeType index) noexcept { return at(index); }
	[[nodiscard]] constexpr auto& operator[](const SizeType index) const noexcept { return at(index); }
	[[nodiscard]] constexpr auto& at(const SizeType index) noexcept { return storage_.dataMutable()[(read_ + index) % capacity()]; }
	[[nodiscard]] constexpr auto& at(const SizeType index) const noexcept { return storage_.data()[(read_ + index) % capacity()]; }

	/// Get the spans container over the reading memory region.
	///
	/// \return The spans container.
	[[nodiscard]] constexpr auto asSpans() const noexcept
	{
		const auto start = read_ % capacity();
		const auto end = write_ % capacity();
		const auto secondSpan = (!empty() && (end <= start)) ? bzd::Span<DataType>{storage_.data(), end} : bzd::Span<DataType>{};

		return bzd::Spans<DataType, 2>{inPlace, asSpanForReading(), secondSpan};
	}

	/// Get the span of contiguous memory containing data starting from the tail.
	/// In other word, this represents the first half of contiguous data to be read.
	///
	/// \return A span containing the contiguous memory region.
	[[nodiscard]] constexpr auto asSpanForReading() const noexcept
	{
		if (empty())
		{
			return bzd::Span<DataType>{};
		}
		const auto start = read_ % capacity();
		const auto end = write_ % capacity();
		return bzd::Span<DataType>{&storage_.data()[start], (end > start) ? (end - start) : (capacity() - start)};
	}

	/// Get the span of contiguous memory containing free slots starting from the head.
	/// In other word, this represents the first half of contiguous free slots.
	///
	/// \return A span containing the contiguous memory region.
	[[nodiscard]] constexpr auto asSpanForWriting() noexcept
	{
		if (full())
		{
			return bzd::Span<DataMutableType>{};
		}
		const auto start = write_ % capacity();
		const auto end = read_ % capacity();
		return bzd::Span<DataMutableType>{&storage_.dataMutable()[start], (end > start) ? (end - start) : (capacity() - start)};
	}

public: // Modifiers.
	/// Consume the ring buffer from the tail.
	constexpr void consume(const bzd::SizeType n) noexcept
	{
		bzd::assert::isTrue(read_ + n <= write_, "Consumer reads passed written pointer.");
		read_ += n;
	}

	/// Produce new data in the ring buffer.
	constexpr void produce(const bzd::SizeType n) noexcept
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
	bzd::SizeType write_{0};
	bzd::SizeType read_{0};
	bzd::BoolType overrun_{false};
};
} // namespace bzd::impl

namespace bzd {
template <class T, SizeType N>
class RingBuffer : public impl::RingBuffer<T, impl::FixedStorage<T, N>>
{
	static_assert((N & (N - 1)) == 0, "The capacity of a RingBuffer must be a power of 2.");

protected:
	using Parent = impl::RingBuffer<T, impl::FixedStorage<T, N>>;
	using Self = RingBuffer<T, N>;
	using StorageType = typename Parent::StorageType;

public: // Constructor.
	constexpr RingBuffer() noexcept = default;
};
} // namespace bzd
