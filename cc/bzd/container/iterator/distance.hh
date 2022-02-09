#pragma once

#include "cc/bzd/container/iterator/traits.hh"
#include "cc/bzd/platform/types.hh"
#include "cc/bzd/type_traits/is_iterator.hh"

namespace bzd::iterator {
template <class Iterator>
requires concepts::forwardIterator<Iterator>
[[nodiscard]] constexpr auto distance(Iterator first, Iterator last) noexcept
{
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
