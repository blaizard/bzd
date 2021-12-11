#pragma once

#include "cc/bzd/type_traits/is_iterator.hh"
#include "cc/bzd/utility/move.hh"

namespace bzd::algorithm {

/// Moves the elements from the range [first, last), to another range ending at d_last.
/// The elements are moved in reverse order (the last element is moved first), but their relative order is preserved.
///
/// \param[in] first The beginning of the first range of elements to be moved.
/// \param[in] last The ending of the first range of elements to be moved.
/// \param[out] result The end of the second range of elements to moved to.
///
/// \return Iterator in the destination range, pointing at the last element moved.
template <class Iterator1, class Iterator2>
constexpr Iterator2 moveBackward(Iterator1 first, Iterator1 last, Iterator2 result) noexcept
{
	static_assert(typeTraits::isIterator<Iterator1> && typeTraits::isIterator<Iterator2>, "Only iterators can be used with moveBackward.");
	static_assert(iterator::isCategory<Iterator1, iterator::BidirectionalTag>, "The iterator must be a bidirectional iterator.");
	static_assert(iterator::isCategory<Iterator2, iterator::BidirectionalTag>, "The iterator must be a bidirectional iterator.");

	while (first != last)
	{
		*(--result) = bzd::move(*(--last));
	}
	return result;
}
} // namespace bzd::algorithm
