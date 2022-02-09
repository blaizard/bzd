#pragma once

#include "cc/bzd/type_traits/is_iterator.hh"
#include "cc/bzd/utility/swap.hh"

namespace bzd::algorithm {

/// Reverses the order of the elements in the range [first, last).
///
/// \param[in,out] first The beginning of the range of elements to be reversed.
/// \param[in,out] last The ending of the range of elements to be reversed.
template <class Iterator>
requires concepts::bidirectionalIterator<Iterator>
constexpr void reverse(Iterator first, Iterator last)
{
	while ((first != last) && (first != --last))
	{
		bzd::swap(*first++, *last);
	}
}
} // namespace bzd::algorithm
