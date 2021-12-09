#pragma once

#include "cc/bzd/container/iterator/traits.hh"
#include "cc/bzd/platform/types.hh"
#include "cc/bzd/type_traits/is_iterator.hh"

namespace bzd::iterator {
template <class Iterator>
[[nodiscard]] constexpr auto distance(Iterator first, Iterator last) noexcept
{
	static_assert(typeTraits::isIterator<Iterator>, "Only iterators can be used with distance.");
	static_assert(isCategory<Iterator, ForwardTag>, "The iterator must be a forward iterator.");

	using DifferenceType = typename Traits<Iterator>::DifferenceType;

	if constexpr (isCategory<Iterator, RandomAccessTag>)
	{
		return static_cast<DifferenceType>(last - first);
	}
	else
	{
		DifferenceType result = 0;
		while (first != last)
		{
			++first;
			++result;
		}
		return result;
	}
}
} // namespace bzd::iterator
