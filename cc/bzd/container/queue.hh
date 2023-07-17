#pragma once

#include "cc/bzd/container/impl/single_linked_pool.hh"
#include "cc/bzd/container/span.hh"
#include "cc/bzd/core/assert/minimal.hh"
#include "cc/bzd/platform/types.hh"
#include "cc/bzd/utility/forward.hh"

namespace bzd::impl {
template <class T, class CapacityType = Size>
class Queue
{
protected:
	using PoolType = SingleLinkedPool<T, CapacityType>;
	using Element = typename PoolType::Element;

public:
	constexpr explicit Queue(const bzd::Span<Element> data) noexcept : pool_{data}, first_{npos}, last_{npos} {}

	constexpr CapacityType capacity() const noexcept { return pool_.capacity(); }

	constexpr bool empty() const noexcept { return (first_ == npos); }

	constexpr T& front() noexcept
	{
		bzd::assert::isTrue(!empty());
		return pool_[first_].container_;
	}
	constexpr const T& front() const noexcept
	{
		bzd::assert::isTrue(!empty());
		return pool_[first_].container_;
	}

	constexpr T& back() noexcept
	{
		bzd::assert::isTrue(!empty());
		return pool_[last_].container_;
	}
	constexpr const T& back() const noexcept
	{
		bzd::assert::isTrue(!empty());
		return pool_[last_].container_;
	}

	// Needed for perfect forwarding
	template <class U = T>
	constexpr T& push(U&& value) noexcept
	{
		auto& item = pool_.reserve();
		const auto index = pool_.getIndex(item);
		if (first_ == npos)
		{
			first_ = index;
			last_ = index;
		}
		else
		{
			pool_[last_].next_ = index;
			last_ = index;
		}

		item.container_ = bzd::forward<T>(value);
		return item.container_;
	}

	constexpr void pop() noexcept
	{
		bzd::assert::isTrue(!empty());

		auto& item = pool_[first_];
		first_ = (first_ == last_) ? npos : item.next_;
		pool_.release(item);
	}

private:
	SingleLinkedPool<T, CapacityType> pool_;
	CapacityType first_;
	CapacityType last_;
};
} // namespace bzd::impl

namespace bzd::interface {
template <class T, class CapacityType = Size>
using Queue = impl::Queue<T, CapacityType>;
}

namespace bzd {
template <class T, Size N, class CapacityType = Size>
class Queue : public interface::Queue<T, CapacityType>
{
private:
	using typename interface::Queue<T, CapacityType>::Element;

public:
	constexpr Queue() : interface::Queue<T, CapacityType>
	{
		{
			data_, N
		}
	}
	{
	}

private:
	Element data_[N];
};
} // namespace bzd
