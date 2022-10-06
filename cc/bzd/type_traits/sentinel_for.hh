#pragma once

#include "cc/bzd/type_traits/iterator.hh"
#include "cc/bzd/type_traits/weakly_equality_comparable_with.hh"

namespace bzd::concepts {
template <class Sentinel, class Iterator>
concept sentinelFor = inputOrOutputIterator<Iterator> && weaklyEqualityComparableWith<Sentinel, Iterator>;
}
