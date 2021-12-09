#pragma once

#include "cc/bzd/utility/swap.hh"
#include "cc/bzd/type_traits/is_iterator.hh"

namespace bzd::algorithm {
template <class Iterator>
constexpr void reverse(Iterator first, Iterator last)
{
	static_assert(typeTraits::isIterator<Iterator>, "Only iterators can be used with reverse.");
	static_assert(iterator::isCategory<Iterator, iterator::BidirectionalTag>, "The iterator must be a bidirectional iterator.");

	while ((first != last) && (first != --last))
	{
		bzd::swap(*first++, *last);
	}
}
} // namespace bzd::algorithm
