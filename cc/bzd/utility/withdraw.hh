#pragma once

#include "cc/bzd/platform/types.hh"
#include "cc/bzd/type_traits/iterator.hh"

namespace bzd {
template <class Iterator, class Distance>
requires concepts::bidirectionalIterator<Iterator>
constexpr void withdraw(Iterator& it, const Distance n) noexcept
{
	using DifferenceType = typeTraits::IteratorDifference<Iterator>;

	auto dist = static_cast<DifferenceType>(n);
	if constexpr (concepts::randomAccessIterator<Iterator>)
	{
		it -= dist;
	}
	else
	{
		while (dist > 0)
		{
			--dist;
			--it;
		}
	}
}
} // namespace bzd
