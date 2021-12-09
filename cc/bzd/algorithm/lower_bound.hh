#pragma once

#include "cc/bzd/type_traits/is_iterator.hh"
#include "cc/bzd/container/iterator/distance.hh"

namespace bzd::algorithm
{

template <class Iterator, class T, class Compare>
constexpr Iterator lowerBound(Iterator first, Iterator last, const T& value, Compare comparison)
{
	static_assert(typeTraits::isIterator<Iterator>, "Only iterators can be used with lowerBound.");
	static_assert(iterator::isCategory<Iterator, iterator::ForwardTag>, "The iterator must be a forward iterator.");

    using DifferenceType = bzd::iterator::Traits<Iterator>::DifferenceType;

    Iterator it;
    DifferenceType count = bzd::iterator::distance(first, last);
    DifferenceType step{};
 
    while (count > 0)
    {
        it = first;
        step = count / 2;
        bzd::iterator::advance(it, step);
        if (comparison(*it, value))
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

}
