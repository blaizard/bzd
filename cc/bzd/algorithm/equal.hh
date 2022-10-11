#pragma once

#include "cc/bzd/type_traits/is_same.hh"
#include "cc/bzd/type_traits/iterator.hh"
#include "cc/bzd/type_traits/predicate.hh"
#include "cc/bzd/type_traits/range.hh"
#include "cc/bzd/type_traits/sentinel_for.hh"
#include "cc/bzd/utility/comparison/equal_to.hh"
#include "cc/bzd/utility/forward.hh"

namespace bzd::algorithm {

/// Returns true if the range [first1, last1) is equal to the range [first2, first2 + (last1 - first1)), and false otherwise.
///
/// \param[in] first1 The beginning of the first range of elements to compare.
/// \param[in] last1 The ending of the first range of elements to compare.
/// \param[in] first2 The beginning of the second range of elements to compare.
/// \param[in] predicate The binary predicate which returns ​true if the elements should be treated as equal.
///
/// \return If the elements in the two ranges are equal, returns true. Otherwise returns false.
template <concepts::forwardIterator Iterator1,
		  concepts::sentinelFor<Iterator1> Sentinel1,
		  concepts::forwardIterator Iterator2,
		  concepts::predicate<typeTraits::IteratorValue<Iterator1>, typeTraits::IteratorValue<Iterator2>> BinaryPredicate =
			  bzd::EqualTo<typeTraits::IteratorValue<Iterator1>>>
[[nodiscard]] constexpr bzd::Bool equal(Iterator1 first1, Sentinel1 last1, Iterator2 first2, BinaryPredicate predicate = BinaryPredicate{})
{
	for (; first1 != last1; ++first1, ++first2)
	{
		if (!predicate(*first1, *first2))
		{
			return false;
		}
	}

	return true;
}

/// \copydoc equal
/// \param[in] last2 The ending of the second range of elements to compare.
template <concepts::forwardIterator Iterator1,
		  concepts::sentinelFor<Iterator1> Sentinel1,
		  concepts::forwardIterator Iterator2,
		  concepts::sentinelFor<Iterator2> Sentinel2,
		  concepts::predicate<typeTraits::IteratorValue<Iterator1>, typeTraits::IteratorValue<Iterator2>> BinaryPredicate =
			  bzd::EqualTo<typeTraits::IteratorValue<Iterator1>>>
[[nodiscard]] constexpr bzd::Bool equal(
	Iterator1 first1, Sentinel1 last1, Iterator2 first2, Sentinel2 last2, BinaryPredicate predicate = BinaryPredicate{})
{
	for (; first1 != last1 && first2 != last2; ++first1, ++first2)
	{
		if (!predicate(*first1, *first2))
		{
			return false;
		}
	}

	return (first1 == last1 && first2 == last2);
}

/// \copydoc equal
/// \param[in] range1 The first range of elements to compare.
/// \param[in] range2 The second range of elements to compare.
template <concepts::forwardRange Range1, concepts::forwardRange Range2, class... Args>
[[nodiscard]] constexpr bzd::Bool equal(Range1&& range1, Range2&& range2, Args&&... args)
{
	if constexpr (concepts::sizedRange<Range1> && concepts::sizedRange<Range2>)
	{
		if (bzd::size(range1) == bzd::size(range2))
		{
			return equal(bzd::begin(range1), bzd::end(range1), bzd::begin(range2), bzd::forward<Args>(args)...);
		}
		return false;
	}
	return equal(bzd::begin(range1), bzd::end(range1), bzd::begin(range2), bzd::end(range2), bzd::forward<Args>(args)...);
}
} // namespace bzd::algorithm
