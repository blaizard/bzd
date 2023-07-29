#pragma once

#include "cc/bzd/algorithm/lower_bound.hh"
#include "cc/bzd/algorithm/upper_bound.hh"
#include "cc/bzd/type_traits/iterator.hh"
#include "cc/bzd/type_traits/range.hh"
#include "cc/bzd/type_traits/sentinel_for.hh"
#include "cc/bzd/utility/ranges/begin.hh"
#include "cc/bzd/utility/ranges/end.hh"
#include "cc/bzd/utility/ranges/size.hh"

namespace bzd::algorithm {

template <class Range>
struct EqualAnyOfRangeValueAdapter
{
	constexpr const typeTraits::RangeValue<Range>& operator()(const typeTraits::RangeValue<Range>& value) const noexcept { return value; }
};

/// Given a sorted range of range, check if any of them equals the tested range.
template <concepts::forwardIterator Iterator,
		  concepts::sentinelFor<Iterator> Sentinel,
		  concepts::forwardRange Range,
		  class RangeValueAdapter = EqualAnyOfRangeValueAdapter<Range>>
auto startsWithAnyOf(Iterator first, Sentinel last, Range range, RangeValueAdapter adapter = RangeValueAdapter{}) noexcept
{
	using RangeValueType = typeTraits::RangeValue<Range>;
	using ValueType = typeTraits::IteratorValue<Iterator>;
	// static_assert(concepts::randomAccessRange<RangeValueType>, "The value type of the range must also be random access range.");

	auto rangeFirst = bzd::begin(range);
	auto rangeLast = bzd::end(range);
	bzd::Size index{0};

	while (!(first == last))
	{
		const auto& value = *first;
		++first;

		rangeFirst =
			bzd::algorithm::lowerBound(rangeFirst, rangeLast, value, [&index, &adapter](const RangeValueType& item, const ValueType& v) {
				const auto& adaptedItem = adapter(item);
				return (index >= bzd::size(adaptedItem)) || (adaptedItem[index] < v);
			});
		rangeLast =
			bzd::algorithm::upperBound(rangeFirst, rangeLast, value, [&index, &adapter](const ValueType& v, const RangeValueType& item) {
				const auto& adaptedItem = adapter(item);
				return (index >= bzd::size(adaptedItem)) || (adaptedItem[index] > v);
			});
		++index;

		if (rangeFirst == rangeLast)
		{
			return bzd::end(range);
		}
		else if (bzd::distance(rangeFirst, rangeLast) == 1 && bzd::size(adapter(*rangeFirst)) == index)
		{
			return rangeFirst;
		}
	}

	return bzd::end(range);
}

/// Given a sorted range of range, check if any of them equals the tested range.
template <concepts::forwardRange Range1, concepts::forwardRange Range2>
auto startsWithAnyOf(Range1&& range1, Range2&& range2) noexcept
{
	return bzd::algorithm::startsWithAnyOf(bzd::begin(range1), bzd::end(range1), bzd::forward<Range2>(range2));
}

} // namespace bzd::algorithm
