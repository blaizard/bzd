#pragma once

#include "cc/bzd/container/storage/fixed.hh"
#include "cc/bzd/core/assert/minimal.hh"
#include "cc/bzd/platform/types.hh"

namespace bzd::impl {
template <class T, class Storage>
class RingBuffer
{
protected:
	using Self = RingBuffer<T, Storage>;
	using StorageType = Storage;

public:
	constexpr explicit RingBuffer() noexcept = default;

	// Cannot be copied nor moved, the ring buffer should be passed by reference instead.
	constexpr RingBuffer(const Self&) noexcept = delete;
	constexpr Self& operator=(const Self&) noexcept = delete;
	constexpr RingBuffer(Self&&) noexcept = delete;
	constexpr Self& operator=(Self&&) noexcept = delete;

public: // Size
	[[nodiscard]] constexpr bzd::BoolType empty() const noexcept { return (write_ - read_) == 0; }

	[[nodiscard]] constexpr bzd::BoolType full() const noexcept { return (write_ - read_) >= capacity(); }

	[[nodiscard]] constexpr bzd::SizeType size() const noexcept { return (full()) ? capacity() : (write_ - read_); }

	[[nodiscard]] constexpr bzd::SizeType capacity() const noexcept { return storage_.size(); }

public: // Accessors
	[[nodiscard]] constexpr auto& operator[](const SizeType index) noexcept { return at(index); }
	[[nodiscard]] constexpr auto& operator[](const SizeType index) const noexcept { return at(index); }
	[[nodiscard]] constexpr auto& at(const SizeType index) noexcept { return storage_.dataMutable()[(read_ + index) % capacity()]; }
	[[nodiscard]] constexpr auto& at(const SizeType index) const noexcept { return storage_.data()[(read_ + index) % capacity()]; }

public: // API
	constexpr void pushBack(const T& value) noexcept
	{
		storage_.dataMutable()[write_ % capacity()] = value;
		++write_;
	}

	constexpr void clear() noexcept
	{
		read_ = 0;
		write_ = 0;
	}

private: // Variables.
	StorageType storage_{};
	bzd::SizeType read_{0};
	bzd::SizeType write_{0};
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
