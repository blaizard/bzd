#pragma once

#include "cc/bzd/platform/types.hh"
#include "cc/bzd/type_traits/iterator.hh"

namespace bzd {
template <class Iterator>
requires concepts::forwardIterator<Iterator>
[[nodiscard]] constexpr auto distance(Iterator first, Iterator last) noexcept
{
	using DifferenceType = typename typeTraits::Iterator<Iterator>::DifferenceType;

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
