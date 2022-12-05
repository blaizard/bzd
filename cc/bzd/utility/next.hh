#pragma once

#include "cc/bzd/type_traits/iterator.hh"
#include "cc/bzd/utility/advance.hh"

namespace bzd {

template <concepts::inputOrOutputIterator Iterator>
constexpr Iterator next(Iterator it, const typeTraits::IteratorDifference<Iterator> n = 1) noexcept
{
	bzd::advance(it, n);
	return it;
}

} // namespace bzd
