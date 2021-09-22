#pragma once

#include "cc/bzd/utility/swap.hh"

namespace bzd::algorithm {
template <class BidirIt>
constexpr void reverse(BidirIt first, BidirIt last)
{
	while ((first != last) && (first != --last))
	{
		bzd::swap(*first++, *last);
	}
}
} // namespace bzd::algorithm
