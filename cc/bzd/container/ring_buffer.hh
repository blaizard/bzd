#pragma once

#include "cc/bzd/container/storage/fixed.hh"
#include "cc/bzd/core/assert/minimal.hh"
#include "cc/bzd/platform/types.hh"
#include "cc/bzd/container/span.hh"

namespace bzd::impl {
/// Ring buffer implementation.
/// |-----|xxxxxxxxxxxx|--------|
/// 0    read         write  capacity
template <class T, class Storage>
class RingBuffer
{
protected:
	using Self = RingBuffer<T, Storage>;
	using StorageType = Storage;
	using DataType = typename StorageType::DataType;
	using DataMutableType = typename StorageType::DataMutableType;

public:
	constexpr explicit RingBuffer() noexcept = default;

	// Cannot be copied nor moved, the ring buffer should be passed by reference instead.
	constexpr RingBuffer(const Self&) noexcept = delete;
	constexpr Self& operator=(const Self&) noexcept = delete;
	constexpr RingBuffer(Self&&) noexcept = delete;
	constexpr Self& operator=(Self&&) noexcept = delete;

public: // Size.
	[[nodiscard]] constexpr bzd::BoolType empty() const noexcept { return (write_ - read_) == 0; }
	[[nodiscard]] constexpr bzd::BoolType full() const noexcept { return (write_ - read_) >= capacity(); }
	[[nodiscard]] constexpr bzd::SizeType size() const noexcept { return (full()) ? capacity() : (write_ - read_); }
	[[nodiscard]] constexpr bzd::SizeType capacity() const noexcept { return storage_.size(); }

public: // Accessors.
	[[nodiscard]] constexpr auto& operator[](const SizeType index) noexcept { return at(index); }
	[[nodiscard]] constexpr auto& operator[](const SizeType index) const noexcept { return at(index); }
	[[nodiscard]] constexpr auto& at(const SizeType index) noexcept { return storage_.dataMutable()[(read_ + index) % capacity()]; }
	[[nodiscard]] constexpr auto& at(const SizeType index) const noexcept { return storage_.data()[(read_ + index) % capacity()]; }

	/// Get the span of contiguous memory containing data starting from the tail.
	/// In other word, this represents the first half of contiguous data to be read.
	[[nodiscard]] constexpr bzd::Span<DataType> asSpanForReading() const noexcept
	{
		if (empty())
		{
			return bzd::Span<DataType>{};
		}
		const auto start = read_ % capacity();
		const auto end = write_ % capacity();
		if (end > start)
		{
			return bzd::Span<DataType>{start, end - start};
		}
		else
		{
			return bzd::Span<DataType>{start, capacity() - start};
		}
	}
/*
	/// Get the span of contiguous memory containing free slots starting from the head.
	/// In other word, this represents the first half of contiguous free slots.
	[[nodiscard]] constexpr bzd::Span<DataMutableType> asSpanForWriting() noexcept
	{
	}
*/
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
		bzd::assert::isTrue(write_ + n - read_ <= capacity(), "Producer overruns consumer.");
		write_ += n;
	}

	constexpr void pushBack(const T& value) noexcept
	{
		storage_.dataMutable()[write_ % capacity()] = value;
		produce(1);
	}

	constexpr void clear() noexcept
	{
		write_ = 0;
		read_ = 0;
	}

private: // Variables.
	StorageType storage_{};
	bzd::SizeType write_{0};
	bzd::SizeType read_{0};
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
