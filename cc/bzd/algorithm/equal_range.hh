#pragma once

#include "cc/bzd/algorithm/lower_bound.hh"
#include "cc/bzd/algorithm/upper_bound.hh"
#include "cc/bzd/container/tuple.hh"
#include "cc/bzd/type_traits/iterator/traits.hh"
#include "cc/bzd/type_traits/range/traits.hh"
#include "cc/bzd/utility/comparison/less.hh"
#include "cc/bzd/utility/forward.hh"

namespace bzd::algorithm {

/// Returns a range containing all elements equivalent to value in the range [first, last).
/// The range [first, last) must be ordered with respect to the comparison object.
///
/// \param[in] first The beginning iterator defining the partially-ordered range to examine.
/// \param[in] last The ending iterator defining the partially-ordered range to examine.
/// \param[in] value The value to compare the elements to.
/// \param[in] comparison Binary predicate which returns ​true if the first argument is less than (i.e. is ordered before) the second.
///
/// \return A pair of iterators defining the wanted range, the first pointing to the first element that is not less than
/// value and the second pointing to the first element greater than value.
/// If there are no elements not less than value, last is returned as the first element. Similarly if there are no elements
/// greater than value, last is returned as the second element.
template <class Iterator, class T, class Compare = bzd::Less<typename typeTraits::Iterator<Iterator>::ValueType>>
requires concepts::forwardIterator<Iterator>
constexpr bzd::Tuple<Iterator, Iterator> equalRange(Iterator first, Iterator last, const T& value, Compare comparison = Compare{})
{
	return bzd::makeTuple(bzd::algorithm::lowerBound(first, last, value, comparison),
						  bzd::algorithm::upperBound(first, last, value, comparison));
}

/// \copydoc equalRange
/// \param[in] first The partially-ordered range to examine.
template <class Range, class... Args>
requires concepts::forwardRange<Range>
constexpr auto equalRange(Range range, Args&&... args)
{
	return equalRange(bzd::begin(range), bzd::end(range), bzd::forward<Args>(args)...);
}

} // namespace bzd::algorithm
