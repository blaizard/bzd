#pragma once

#include "cc/bzd/container/iterator/advance.hh"
#include "cc/bzd/container/iterator/distance.hh"
#include "cc/bzd/type_traits/is_iterator.hh"
#include "cc/bzd/utility/comparison/less.hh"

namespace bzd::algorithm {

template <class Iterator, class T, class Compare = bzd::Less<typename Iterator::ValueType>>
constexpr Iterator upperBound(Iterator first, Iterator last, const T& value, Compare comparison = Compare{})
{
	static_assert(typeTraits::isIterator<Iterator>, "Only iterators can be used with upperBound.");
	static_assert(iterator::isCategory<Iterator, iterator::ForwardTag>, "The iterator must be a forward iterator.");

	using DifferenceType = typename bzd::iterator::Traits<Iterator>::DifferenceType;

	Iterator it;
	DifferenceType count = bzd::iterator::distance(first, last);
	DifferenceType step{};

	while (count > 0)
	{
		it = first;
		step = count / 2;
		bzd::iterator::advance(it, step);
		if (!comparison(value, *it))
		{
			first = ++it;
			count -= step + 1;
		}
		else
		{
			count = step;
		}
	}
	return first;
}

} // namespace bzd::algorithm
