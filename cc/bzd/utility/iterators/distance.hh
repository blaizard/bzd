#pragma once

#include "cc/bzd/platform/types.hh"
#include "cc/bzd/type_traits/iterator.hh"
#include "cc/bzd/type_traits/sentinel_for.hh"

namespace bzd {
template <concepts::forwardIterator Iterator, concepts::sentinelFor<Iterator> Sentinel>
[[nodiscard]] constexpr auto distance(Iterator first, Sentinel last) noexcept
{
	using DifferenceType = typeTraits::IteratorDifference<Iterator>;

	if constexpr (concepts::randomAccessIterator<Iterator>)
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
} // namespace bzd
