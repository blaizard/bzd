#pragma once

#include "cc/bzd/type_traits/is_iterator.hh"

namespace bzd::algorithm {
template <class Iterator, class T>
constexpr void fill(Iterator first, Iterator last, const T& value)
{
	static_assert(typeTraits::isIterator<Iterator>, "Only iterators can be used with fill.");
	static_assert(iterator::isCategory<Iterator, iterator::ForwardTag>, "The iterator must be a forward iterator.");

	for (; first != last; ++first)
	{
		*first = value;
	}
}
} // namespace bzd::algorithm
