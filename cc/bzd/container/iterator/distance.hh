#pragma once

#include "cc/bzd/platform/types.hh"
#include "cc/bzd/type_traits/is_base_of.hh"

namespace bzd::iterator {
template <class It>
[[nodiscard]] constexpr typename It::DifferenceType distance(It first, It last) noexcept
{
	static_assert(bzd::typeTraits::isBaseOf<Iterator, It>, "Only iterators can be used with distance.");

	if constexpr (bzd::typeTraits::isBaseOf<RandomAccessTag, typename It::Category>)
	{
		return last - first;
	}
	else
	{
		typename It::DifferenceType result = 0;
		while (first != last)
		{
			++first;
			++result;
		}
		return result;
	}
}
} // namespace bzd::iterator
