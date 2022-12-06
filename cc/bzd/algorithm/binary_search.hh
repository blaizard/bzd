#pragma once

#include "cc/bzd/algorithm/lower_bound.hh"
#include "cc/bzd/type_traits/iterator.hh"
#include "cc/bzd/type_traits/predicate.hh"
#include "cc/bzd/type_traits/range.hh"
#include "cc/bzd/type_traits/sentinel_for.hh"
#include "cc/bzd/utility/comparison/less.hh"
#include "cc/bzd/utility/forward.hh"

namespace bzd::algorithm {

/// Checks if an element equivalent to value appears within the range [first, last).
/// For binarySearch to succeed, the range [first, last) must be ordered with respect to the comparison operator.
///
/// \param[in] first The begin iterator of the range of elements to examine.
/// \param[in] last The end iterator of the range of elements to examine.
/// \param[in] value The value to compare the elements to.
/// \param[in] comparison A binary predicate which returns ​true if the first argument is less than (i.e. is ordered before) the second.
///
/// \return The iterator pointing to the element if the value is found, \c last otherwise.
template <concepts::forwardIterator Iterator,
		  concepts::sentinelFor<Iterator> Sentinel,
		  class T,
		  concepts::predicate<T, typeTraits::IteratorValue<Iterator>> Compare = bzd::Less<typeTraits::IteratorValue<Iterator>>>
[[nodiscard]] constexpr Iterator binarySearch(Iterator first, Sentinel last, const T& value, Compare comparison = Compare{})
{
	first = bzd::algorithm::lowerBound(first, last, value, comparison);
	if ((first != last) && !(comparison(value, *first)))
	{
		return first;
	}

	return last;
}

/// \copydoc binarySearch
/// \param[in] range The range of elements to examine.
template <concepts::forwardRange Range, class... Args>
[[nodiscard]] constexpr auto binarySearch(Range&& range, Args&&... args)
{
	return bzd::algorithm::binarySearch(bzd::begin(range), bzd::end(range), bzd::forward<Args>(args)...);
}

} // namespace bzd::algorithm
