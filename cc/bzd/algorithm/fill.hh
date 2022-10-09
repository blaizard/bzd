#pragma once

#include "cc/bzd/type_traits/iterator.hh"
#include "cc/bzd/type_traits/range.hh"
#include "cc/bzd/type_traits/sentinel_for.hh"
#include "cc/bzd/utility/forward.hh"

namespace bzd::algorithm {

/// Assigns the given value to the elements in the range [first, last).
///
/// \param[in,out] first The beginning of the range of elements to modify.
/// \param[in,out] last The ending of the range of elements to modify.
/// \param[in] value The value to be assigned.
template <concepts::forwardIterator Iterator, concepts::sentinelFor<Iterator> Sentinel, class T>
constexpr void fill(Iterator first, Sentinel last, const T& value)
{
	for (; first != last; ++first)
	{
		*first = value;
	}
}

/// \copydoc fill
/// \param[in,out] range The range of elements to modify.
template <concepts::forwardRange Range, class... Args>
constexpr void fill(Range&& range, Args&&... args)
{
	fill(bzd::begin(range), bzd::end(range), bzd::forward<Args>(args)...);
}

} // namespace bzd::algorithm
