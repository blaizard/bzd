#pragma once

#include "cc/bzd/type_traits/iterator.hh"
#include "cc/bzd/type_traits/range.hh"
#include "cc/bzd/type_traits/sentinel_for.hh"
#include "cc/bzd/utility/comparison/equal_to.hh"
#include "cc/bzd/utility/forward.hh"

namespace bzd::algorithm {

/// Search for the first occurence of the sequence of elements [first2, last2[ in the range [first1, last1[.
///
/// \param[in] first1 The beginning of the range of elements to examine.
/// \param[in] last1 The ending of the range of elements to examine.
/// \param[in] first2 The beginning of the range of elements to search.
/// \param[in] last2 The ending of the range of elements to search.
/// \param[in] predicate The binary predicate which returns \c ​true for the same elements.
/// \return Iterator to the beginning of first occurrence of the sequence [first2, last2[ in the range [first1, last1[.
/// If no such occurrence is found, last1 is returned.
template <concepts::forwardIterator Iterator1,
		  concepts::sentinelFor<Iterator1> Sentinel1,
		  concepts::forwardIterator Iterator2,
		  concepts::sentinelFor<Iterator2> Sentinel2,
		  class BinaryPredicate =
			  bzd::EqualTo<typename typeTraits::Iterator<Iterator1>::ValueType, typename typeTraits::Iterator<Iterator2>::ValueType>>
[[nodiscard]] constexpr Iterator1 search(
	Iterator1 first1, Sentinel1 last1, Iterator2 first2, Sentinel2 last2, BinaryPredicate predicate = BinaryPredicate{}) noexcept
{
	while (true)
	{
		auto it1 = first1;
		for (auto it2 = first2;; ++it1, ++it2)
		{
			if (it2 == last2)
			{
				return first1;
			}
			if (it1 == last1)
			{
				return last1;
			}
			if (!predicate(*it1, *it2))
			{
				break;
			}
		}
		++first1;
	}
}

/// \copydoc search
/// \param[in] range1 The range of elements to examine.
/// \param[in] range2 The range of elements to search.
template <concepts::forwardRange Range1, concepts::forwardRange Range2, class... Args>
constexpr auto search(Range1&& range1, Range2&& range2, Args&&... args)
{
	return search(bzd::begin(range1), bzd::end(range1), bzd::begin(range2), bzd::end(range2), bzd::forward<Args>(args)...);
}

} // namespace bzd::algorithm
