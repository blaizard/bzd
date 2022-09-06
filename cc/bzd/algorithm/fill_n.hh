#pragma once

#include "cc/bzd/type_traits/iterator.hh"
#include "cc/bzd/utility/forward.hh"

namespace bzd::algorithm {

/// Assigns the given value to `count` elements of the sequence pointed by `first`.
///
/// \param[in,out] first The beginning of the range of elements to modify.
/// \param[in] count The number of elements to fill.
/// \param[in] value The value to be assigned.
template <class Iterator, class T>
requires concepts::forwardIterator<Iterator>
constexpr void fillN(Iterator first, Size count, const T& value)
{
	while (count > 0)
	{
		*first = value;
		++first;
		--count;
	}
}

} // namespace bzd::algorithm
