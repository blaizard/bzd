#pragma once

#include "cc/bzd/type_traits/iterator.hh"
#include "cc/bzd/utility/advance.hh"

namespace bzd {

template <concepts::bidirectionalIterator Iterator>
constexpr Iterator prev(Iterator it, const typeTraits::IteratorDifference<Iterator> n = 1) noexcept
{
	bzd::advance(it, -n);
	return it;
}

} // namespace bzd
