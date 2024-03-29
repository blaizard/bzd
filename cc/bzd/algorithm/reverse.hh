#pragma once

#include "cc/bzd/type_traits/iterator.hh"
#include "cc/bzd/type_traits/range.hh"
#include "cc/bzd/type_traits/sentinel_for.hh"
#include "cc/bzd/utility/swap.hh"

namespace bzd::algorithm {

/// Reverses the order of the elements in the range [first, last).
///
/// \param[in,out] first The beginning of the range of elements to be reversed.
/// \param[in,out] last The ending of the range of elements to be reversed.
template <concepts::bidirectionalIterator Iterator, concepts::sentinelFor<Iterator> Sentinel>
constexpr void reverse(Iterator first, Sentinel last) noexcept
{
	while ((first != last) && (first != --last))
	{
		bzd::swap(*first++, *last);
	}
}

/// \copydoc reverse
/// \param[in] range The range of elements to be reversed.
template <concepts::bidirectionalRange Range>
constexpr void reverse(Range&& range) noexcept
{
	bzd::algorithm::reverse(bzd::begin(range), bzd::end(range));
}

} // namespace bzd::algorithm
