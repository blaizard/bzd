#pragma once

#include "cc/bzd/container/optional.hh"
#include "cc/bzd/core/async/forward.hh"
#include "cc/bzd/platform/types.hh"
#include "cc/bzd/type_traits/iterator.hh"
#include "cc/bzd/type_traits/range.hh"
#include "cc/bzd/type_traits/sentinel_for.hh"
#include "cc/bzd/utility/iterators/sentinel.hh"

namespace bzd::iterator {

template <class Iterator, concepts::sentinelFor<Iterator> Sentinel = Iterator>
class ContainerOfIterables : public typeTraits::IteratorBase
{
public: // Traits.
	using Self = ContainerOfIterables<Iterator, Sentinel>;
	using ContainerValueType = typeTraits::IteratorValue<Iterator>;
	using NestedIterator = typeTraits::RangeIterator<ContainerValueType>;
	using DifferenceType = typeTraits::IteratorDifference<NestedIterator>;
	using ValueType = typeTraits::IteratorValue<NestedIterator>;
	static constexpr auto category = typeTraits::IteratorCategory::forward;

public: // Constructors.
	constexpr ContainerOfIterables(Iterator it, Sentinel end) noexcept : it_{it}, end_{end}, nested_{} {}

	constexpr ContainerOfIterables(const Self&) noexcept = default;
	constexpr Self& operator=(const Self&) noexcept = default;
	constexpr ContainerOfIterables(Self&&) noexcept = default;
	constexpr Self& operator=(Self&&) noexcept = default;

public: // Modifiers.
	constexpr Self& operator++() noexcept
	requires(concepts::syncIterator<Iterator>)
	{
		do
		{
			if (nested_)
			{
				++(nested_.valueMutable());
			}
			else
			{
				nested_ = bzd::begin(*it_);
			}

			if (nested_.value() != bzd::end(*it_))
			{
				break;
			}
			++it_;
			nested_.reset();
		} while (it_ != end_);

		return *this;
	}

	bzd::Async<> operator++() noexcept
	requires(concepts::asyncIterator<Iterator>);

	constexpr Self operator++(int) noexcept
	requires(concepts::syncIterator<Iterator>)
	{
		Self it{*this};
		++(*this);
		return it;
	}

public: // Comparators.
	[[nodiscard]] constexpr Bool operator==(const iterator::Sentinel<Self>&) const noexcept { return (it_ == end_); }
	[[nodiscard]] constexpr Bool operator!=(const iterator::Sentinel<Self>&) const noexcept { return (it_ != end_); }

public: // Accessors.
	[[nodiscard]] constexpr ValueType& operator*() const { return *(nested_.value()); }
	[[nodiscard]] constexpr ValueType* operator->() const { return &(*(nested_.value())); }

private:
	Iterator it_;
	Sentinel end_;
	bzd::Optional<NestedIterator> nested_;
};

} // namespace bzd::iterator
