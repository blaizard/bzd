#pragma once

#include "cc/bzd/type_traits/iterator.hh"
#include "cc/bzd/type_traits/range.hh"
#include "cc/bzd/type_traits/sentinel_for.hh"
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
template <concepts::forwardIterator Iterator1, concepts::sentinelFor<Iterator1> Sentinel1, concepts::outputIterator Iterator2>
constexpr Iterator2 move(Iterator1 first, Sentinel1 last, Iterator2 result) noexcept
{
	while (first != last)
	{
		*result = bzd::move(*first);
		++result;
		++first;
	}
	return result;
}

/// \copydoc move
/// \param[in] range1 The first range of elements to be moved.
/// \param[out] range2 The second range of elements to moved to.
template <concepts::forwardRange Range1, concepts::outputRange Range2>
constexpr auto move(Range1&& range1, Range2&& range2) noexcept
{
	return bzd::algorithm::move(bzd::begin(range1), bzd::end(range1), bzd::begin(range2));
}

} // namespace bzd::algorithm
