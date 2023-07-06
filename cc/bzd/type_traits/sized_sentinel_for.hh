#pragma once

#include "cc/bzd/type_traits/is_same.hh"
#include "cc/bzd/type_traits/iterator.hh"
#include "cc/bzd/type_traits/sentinel_for.hh"

namespace bzd::concepts {
template <class Sentinel, class Iterator>
concept sizedSentinelFor = sentinelFor<Sentinel, Iterator> && requires(const Iterator& i, const Sentinel& s) {
	{
		s - i
	} -> sameAs<typeTraits::IteratorDifference<Iterator>>;
	{
		i - s
	} -> sameAs<typeTraits::IteratorDifference<Iterator>>;
};
} // namespace bzd::concepts
