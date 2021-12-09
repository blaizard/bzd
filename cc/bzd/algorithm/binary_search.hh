#pragma once

#include "cc/bzd/type_traits/is_iterator.hh"
#include "cc/bzd/utility/comparison/less.hh"
#include "cc/bzd/algorithm/lower_bound.hh"

namespace bzd::algorithm {
template <class Iterator, class T, class Compare = bzd::Less<typename Iterator::ValueType>>
[[nodiscard]] constexpr bzd::BoolType binarySearch(Iterator first, Iterator last, const T& value, Compare comparison = Compare{})
{
	static_assert(typeTraits::isIterator<Iterator>, "Only iterators can be used with binarySearch.");
	static_assert(iterator::isCategory<Iterator, iterator::ForwardTag>, "The iterator must be a forward iterator.");

    first = lowerBound(first, last, value, comparison);
    return (!(first == last) && !(comparison(value, *first)));
}

} // namespace bzd::algorithm
