#pragma once

#include "cc/bzd/container/span.hh"
#include "cc/bzd/core/assert/minimal.hh"
#include "cc/bzd/platform/types.hh"

#include <iostream>

namespace bzd::impl {
template <class T, class CapacityType = SizeType>
struct SingleLinkedPoolElement
{
	T container_;
	CapacityType next_;
};

template <class T, class CapacityType = SizeType>
class SingleLinkedPool : public bzd::Span<SingleLinkedPoolElement<T, CapacityType>>
{
public:
	using Element = SingleLinkedPoolElement<T, CapacityType>;
	using Parent = bzd::Span<Element>;

public:
	constexpr explicit SingleLinkedPool(const bzd::Span<Element> data) noexcept : Parent(data), free_(0)
	{
		bzd::assert::isTrue(Parent::size() > 0);

		// Build the free list
		for (CapacityType i = 0; i < Parent::size() - 1; ++i)
		{
			Parent::at(i).next_ = i + 1;
		}
		Parent::back().next_ = npos;
	}

	constexpr CapacityType capacity() const noexcept { return Parent::size(); }

	constexpr bool empty() const noexcept { return (free_ == npos); }

	/// Release an element from the pool
	constexpr void release(Element& element) noexcept
	{
		const auto index = getIndex(element);
		element.next_ = free_;
		free_ = index;
	}

	/// Reserve an element from the free list (if any)
	constexpr Element& reserve() noexcept
	{
		bzd::assert::isTrue(!empty());

		Element& item = Parent::at(free_);
		free_ = item.next_;
		return item;
	}

	constexpr CapacityType getIndex(const Element& element) const noexcept
	{
		bzd::assert::isTrue(reinterpret_cast<bzd::IntPtrType>(&element) >= reinterpret_cast<bzd::IntPtrType>(Parent::data()));
		bzd::assert::isTrue(reinterpret_cast<bzd::IntPtrType>(&element) <
							reinterpret_cast<bzd::IntPtrType>(Parent::data() + Parent::size()));
		return (&element - Parent::data());
	}

	void toStream(std::ostream& os)
	{
		os << "free=";
		auto current = free_;
		while (current != npos)
		{
			os << current << ",";
			current = Parent::at(current).next_;
		}
	}

private:
	CapacityType free_;
};
} // namespace bzd::impl
