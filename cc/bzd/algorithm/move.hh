#pragma once

#include "cc/bzd/type_traits/iterator.hh"
#include "cc/bzd/type_traits/range.hh"
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
requires concepts::forwardIterator<Iterator1> && concepts::forwardIterator<Iterator2>
constexpr Iterator2 move(Iterator1 first, Iterator1 last, Iterator2 result) noexcept
{
	while (first != last)
	{
		*result++ = bzd::move(*first++);
	}
	return result;
}

/// \copydoc move
/// \param[in] range1 The first range of elements to be moved.
/// \param[out] range2 The second range of elements to moved to.
template <class Range1, class Range2>
requires concepts::forwardRange<Range1> && concepts::forwardRange<Range2>
constexpr auto move(Range1&& range1, Range2&& range2) noexcept
{
	return move(bzd::begin(range1), bzd::end(range1), bzd::begin(range2));
}

} // namespace bzd::algorithm
