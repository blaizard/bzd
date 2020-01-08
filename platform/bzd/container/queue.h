#pragma once

#include "bzd/container/impl/single_linked_pool.h"
#include "bzd/container/span.h"
#include "bzd/core/assert.h"
#include "bzd/types.h"
#include "bzd/utility/forward.h"

namespace bzd {
namespace impl {
template <class T, class CapacityType = SizeType>
class Queue
{
protected:
	using PoolType = SingleLinkedPool<T, CapacityType>;
	using Element = typename PoolType::Element;

public:
	constexpr explicit Queue(const bzd::Span<Element> data) noexcept : pool_(data), first_(PoolType::npos), last_(PoolType::npos) {}

	constexpr CapacityType capacity() const noexcept { return pool_.capacity(); }

	constexpr bool empty() const noexcept { return (first_ == PoolType::npos); }

	constexpr T &front() noexcept
	{
		bzd::assert::isTrue(!empty());
		return pool_[first_].container_;
	}
	constexpr const T &front() const noexcept
	{
		bzd::assert::isTrue(!empty());
		return pool_[first_].container_;
	}

	constexpr T &back() noexcept
	{
		bzd::assert::isTrue(!empty());
		return pool_[last_].container_;
	}
	constexpr const T &back() const noexcept
	{
		bzd::assert::isTrue(!empty());
		return pool_[last_].container_;
	}

	constexpr void push(T &&value) noexcept
	{
		auto &item = pool_.reserve();
		const auto index = pool_.getIndex(item);
		if (first_ == PoolType::npos)
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
	}

	constexpr void pop() noexcept
	{
		bzd::assert::isTrue(!empty());

		auto &item = pool_[first_];
		first_ = (first_ == last_) ? PoolType::npos : item.next_;
		pool_.release(item);
	}

private:
	SingleLinkedPool<T, CapacityType> pool_;
	CapacityType first_;
	CapacityType last_;
};
} // namespace impl

namespace interface {
template <class T, class CapacityType = SizeType>
using Queue = impl::Queue<T, CapacityType>;
}

/**
 */
template <class T, SizeType N, class CapacityType = SizeType>
class Queue : public interface::Queue<T, CapacityType>
{
private:
	using typename interface::Queue<T, CapacityType>::Element;

public:
	constexpr Queue() : interface::Queue<T, CapacityType>({data_, N}) {}

private:
	Element data_[N];
};
} // namespace bzd
