#pragma once

#include "cc/bzd/type_traits/is_iterator.hh"
#include "cc/bzd/utility/move.hh"

namespace bzd::algorithm {

/// Moves the elements in the range [first, last), to another range beginning at result, starting from first and proceeding to last - 1.
/// After this operation the elements in the moved-from range will still contain valid values of the appropriate type,
/// but not necessarily the same values as before the move.
///
/// \param[in] first The beginning of the first range of elements to be moved.
/// \param[in] last The ending of the first range of elements to be moved.
/// \param[out] result The begining of the second range of elements to moved to.
///
/// \return Output iterator to the element past the last element moved (result + (last - first)).
template <class Iterator1, class Iterator2>
constexpr Iterator2 move(Iterator1 first, Iterator1 last, Iterator2 result) noexcept
{
	static_assert(typeTraits::isIterator<Iterator1> && typeTraits::isIterator<Iterator2>, "Only iterators can be used with move.");
	static_assert(iterator::isCategory<Iterator1, iterator::ForwardTag>, "The iterator must be a forward iterator.");
	static_assert(iterator::isCategory<Iterator2, iterator::ForwardTag>, "The iterator must be a forward iterator.");

	while (first != last)
	{
		*result++ = bzd::move(*first++);
	}
	return result;
}
} // namespace bzd::algorithm
