#pragma once

#include "cc/bzd/container/iterator/traits.hh"
#include "cc/bzd/platform/types.hh"
#include "cc/bzd/type_traits/is_iterator.hh"

namespace bzd::iterator {
template <class Iterator, class Distance>
requires concepts::inputIterator<Iterator>
constexpr void advance(Iterator& it, const Distance n) noexcept
{
	using DifferenceType = typename Traits<Iterator>::DifferenceType;

	auto dist = static_cast<DifferenceType>(n);
	if constexpr (isCategory<Iterator, RandomAccessTag>)
	{
		it += dist;
	}
	else
	{
		while (dist > 0)
		{
			--dist;
			++it;
		}
		if constexpr (isCategory<Iterator, BidirectionalTag>)
		{
			while (dist < 0)
			{
				++dist;
				--it;
			}
		}
	}
}
} // namespace bzd::iterator
