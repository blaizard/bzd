#pragma once

#include "cc/bzd/algorithm/lower_bound.hh"
#include "cc/bzd/algorithm/upper_bound.hh"
#include "cc/bzd/container/tuple.hh"
#include "cc/bzd/type_traits/is_iterator.hh"
#include "cc/bzd/utility/comparison/less.hh"

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
template <class Iterator, class T, class Compare = bzd::Less<typename iterator::Traits<Iterator>::ValueType>>
constexpr bzd::Tuple<Iterator, Iterator> equalRange(Iterator first, Iterator last, const T& value, Compare comparison = Compare{})
{
	static_assert(typeTraits::isIterator<Iterator>, "Only iterators can be used with lowerBound.");
	static_assert(iterator::isCategory<Iterator, iterator::ForwardTag>, "The iterator must be a forward iterator.");

	return bzd::makeTuple(bzd::algorithm::lowerBound(first, last, value, comparison),
						  bzd::algorithm::upperBound(first, last, value, comparison));
}

} // namespace bzd::algorithm
