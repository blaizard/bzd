#pragma once

#include "cc/bzd/platform/types.hh"
#include "cc/bzd/type_traits/iterator/traits.hh"

namespace bzd {
template <class Iterator, class Distance>
requires concepts::inputIterator<Iterator>
constexpr void advance(Iterator& it, const Distance n) noexcept
{
	using DifferenceType = typename typeTraits::Iterator<Iterator>::DifferenceType;

	auto dist = static_cast<DifferenceType>(n);
	if constexpr (concepts::randomAccessIterator<Iterator>)
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
		if constexpr (concepts::bidirectionalIterator<Iterator>)
		{
			while (dist < 0)
			{
				++dist;
				--it;
			}
		}
	}
}
} // namespace bzd
