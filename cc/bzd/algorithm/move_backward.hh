#pragma once

#include "cc/bzd/type_traits/iterator/traits.hh"
#include "cc/bzd/type_traits/range/traits.hh"
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
requires concepts::bidirectionalIterator<Iterator1> && concepts::bidirectionalIterator<Iterator2>
constexpr Iterator2 moveBackward(Iterator1 first, Iterator1 last, Iterator2 result) noexcept
{
	while (first != last)
	{
		*(--result) = bzd::move(*(--last));
	}
	return result;
}

/// \copydoc moveBackward
/// \param[in] range1 The first range of elements to be moved.
/// \param[out] range2 The second range of elements to moved to.
template <class Range1, class Range2>
requires concepts::bidirectionalRange<Range1> && concepts::bidirectionalRange<Range2>
constexpr auto moveBackward(Range1&& range1, Range2&& range2) noexcept
{
	return moveBackward(bzd::begin(range1), bzd::end(range1), bzd::begin(range2));
}

} // namespace bzd::algorithm
