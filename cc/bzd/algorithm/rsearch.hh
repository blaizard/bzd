#pragma once

#include "cc/bzd/algorithm/search.hh"
#include "cc/bzd/type_traits/iterator.hh"
#include "cc/bzd/type_traits/range.hh"
#include "cc/bzd/utility/comparison/equal_to.hh"
#include "cc/bzd/utility/forward.hh"
#include "cc/bzd/utility/withdraw.hh"

namespace bzd::algorithm {

/// Search for the last occurence of the sequence of elements [first2, last2[ in the range [first1, last1[.
///
/// \param[in] first1 The beginning of the range of elements to examine.
/// \param[in] last1 The ending of the range of elements to examine.
/// \param[in] first2 The beginning of the range of elements to search.
/// \param[in] last2 The ending of the range of elements to search.
/// \param[in] predicate The binary predicate which returns \c ​true for the same elements.
/// \return Iterator to the beginning of last occurrence of the sequence [first2, last2[ in the range [first1, last1[.
/// If no such occurrence is found, last1 is returned.
template <class Iterator1,
		  class Iterator2,
		  class BinaryPredicate =
			  bzd::EqualTo<typename typeTraits::Iterator<Iterator1>::ValueType, typename typeTraits::Iterator<Iterator2>::ValueType>>
requires concepts::forwardIterator<Iterator1> && concepts::forwardIterator<Iterator2>
[[nodiscard]] constexpr Iterator1 rsearch(
	Iterator1 first1, Iterator1 last1, Iterator2 first2, Iterator2 last2, BinaryPredicate predicate = BinaryPredicate{}) noexcept
{
	if (first2 == last2)
	{
		return last1;
	}

	auto result{last1};
	if constexpr (concepts::bidirectionalIterator<Iterator1>)
	{
		if (first1 == last1)
		{
			return last1;
		}

		auto it{last1};
		do
		{
			auto it1 = --it;
			for (auto it2 = first2;; ++it1, ++it2)
			{
				if (it2 == last2)
				{
					return it;
				}
				if (it1 == last1)
				{
					break;
				}
				if (!predicate(*it1, *it2))
				{
					break;
				}
			}

		} while (it != first1);
	}
	else
	{
		while (true)
		{
			auto new_result = bzd::algorithm::search(first1, last1, first2, last2, predicate);
			if (new_result == last1)
			{
				break;
			}
			else
			{
				result = new_result;
				first1 = result;
				++first1;
			}
		}
	}

	return result;
}

/// \copydoc rsearch
/// \param[in] range1 The range of elements to examine.
/// \param[in] range2 The range of elements to search.
template <class Range1, class Range2, class... Args>
requires concepts::forwardRange<Range1> && concepts::forwardRange<Range2>
constexpr auto rsearch(Range1&& range1, Range2&& range2, Args&&... args)
{
	return rsearch(bzd::begin(range1), bzd::end(range1), bzd::begin(range2), bzd::end(range2), bzd::forward<Args>(args)...);
}

} // namespace bzd::algorithm