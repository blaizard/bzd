#pragma once

#include "cc/bzd/type_traits/is_same.hh"
#include "cc/bzd/type_traits/iterator.hh"
#include "cc/bzd/type_traits/predicate.hh"
#include "cc/bzd/type_traits/range.hh"
#include "cc/bzd/type_traits/sentinel_for.hh"
#include "cc/bzd/utility/comparison/less.hh"
#include "cc/bzd/utility/forward.hh"
#include "cc/bzd/utility/swap.hh"

namespace bzd::algorithm {

/// Checks if the first range [first1, last1) is lexicographically less than the second range [first2, last2).
///
/// \param[in] first1 The beginning of the first range of elements to examine
/// \param[in] last1 The ending of the first range of elements to examine
/// \param[in] first2 The beginning of the second range of elements to examine
/// \param[in] last2 The ending of the second range of elements to examine
/// \param[in] comparison The comparison function object which returns ​true if the first argument is less than the second.
///
/// \return true if the first range is lexicographically less than the second.
template <concepts::forwardIterator Iterator1,
		  concepts::sentinelFor<Iterator1> Sentinel1,
		  concepts::forwardIterator Iterator2,
		  concepts::sentinelFor<Iterator2> Sentinel2,
		  concepts::predicate<typeTraits::IteratorValue<Iterator1>, typeTraits::IteratorValue<Iterator2>> Compare =
			  bzd::Less<typeTraits::IteratorValue<Iterator1>>>
[[nodiscard]] constexpr bzd::Bool lexicographicalCompare(
	Iterator1 first1, Sentinel1 last1, Iterator2 first2, Sentinel2 last2, Compare comparison = Compare{})
{
	static_assert(typeTraits::isSame<typeTraits::IteratorValue<Iterator1>, typeTraits::IteratorValue<Iterator2>>,
				  "Value types of both iterators must match.");

	for (; (first1 != last1) && (first2 != last2); ++first1, ++first2)
	{
		if (comparison(*first1, *first2))
		{
			return true;
		}
		if (comparison(*first2, *first1))
		{
			return false;
		}
	}

	return (first1 == last1) && (first2 != last2);
}

/// \copydoc lexicographicalCompare
/// \param[in] range1 The first range of elements to examine
/// \param[in] range2 The second range of elements to examine
template <concepts::forwardRange Range1, concepts::forwardRange Range2, class... Args>
[[nodiscard]] constexpr auto lexicographicalCompare(Range1&& range1, Range2&& range2, Args&&... args)
{
	return bzd::algorithm::lexicographicalCompare(bzd::begin(range1),
												  bzd::end(range1),
												  bzd::begin(range2),
												  bzd::end(range2),
												  bzd::forward<Args>(args)...);
}

} // namespace bzd::algorithm
