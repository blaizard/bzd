#pragma once

#include "cc/bzd/type_traits/is_same.hh"
#include "cc/bzd/type_traits/iterator/traits.hh"
#include "cc/bzd/type_traits/range/traits.hh"
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
template <class Iterator1, class Iterator2, class BinaryPredicate = bzd::EqualTo<typename typeTraits::Iterator<Iterator1>::ValueType>>
requires concepts::forwardIterator<Iterator1> && concepts::forwardIterator<Iterator2>
[[nodiscard]] constexpr bzd::BoolType equal(Iterator1 first1,
											Iterator1 last1,
											Iterator2 first2,
											BinaryPredicate predicate = BinaryPredicate{})
{
	static_assert(
		typeTraits::isSame<typename typeTraits::Iterator<Iterator1>::ValueType, typename typeTraits::Iterator<Iterator2>::ValueType>,
		"Value types of both iterators must match.");

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
template <class Iterator1, class Iterator2, class BinaryPredicate = bzd::EqualTo<typename typeTraits::Iterator<Iterator1>::ValueType>>
requires concepts::forwardIterator<Iterator1> && concepts::forwardIterator<Iterator2>
[[nodiscard]] constexpr bzd::BoolType equal(Iterator1 first1,
											Iterator1 last1,
											Iterator2 first2,
											Iterator2 last2,
											BinaryPredicate predicate = BinaryPredicate{})
{
	static_assert(
		typeTraits::isSame<typename typeTraits::Iterator<Iterator1>::ValueType, typename typeTraits::Iterator<Iterator2>::ValueType>,
		"Value types of both iterators must match.");

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
template <class Range1, class Range2, class... Args>
requires concepts::forwardRange<Range1> && concepts::forwardRange<Range2>
[[nodiscard]] constexpr auto equal(Range1&& range1, Range2&& range2, Args&&... args)
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
