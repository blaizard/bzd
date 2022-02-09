#pragma once

#include "cc/bzd/type_traits/is_iterator.hh"
#include "cc/bzd/type_traits/is_same.hh"
#include "cc/bzd/utility/comparison/equal_to.hh"

namespace bzd::algorithm {

/// Returns true if the range [first1, last1) is equal to the range [first2, first2 + (last1 - first1)), and false otherwise.
///
/// \param[in] first1 The beginning of the first range of elements to compare.
/// \param[in] last1 The ending of the first range of elements to compare.
/// \param[in] first2 The beginning of the second range of elements to compare.
/// \param[in] predicate The binary predicate which returns ​true if the elements should be treated as equal.
///
/// \return If the elements in the two ranges are equal, returns true. Otherwise returns false.
template <class Iterator1, class Iterator2, class BinaryPredicate = bzd::EqualTo<typename iterator::Traits<Iterator1>::ValueType>>
requires concepts::forwardIterator<Iterator1> && concepts::forwardIterator<Iterator2>
[[nodiscard]] constexpr bzd::BoolType equal(Iterator1 first1,
											Iterator1 last1,
											Iterator2 first2,
											BinaryPredicate predicate = BinaryPredicate{})
{
	static_assert(typeTraits::isSame<typename iterator::Traits<Iterator1>::ValueType, typename iterator::Traits<Iterator2>::ValueType>,
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
} // namespace bzd::algorithm
