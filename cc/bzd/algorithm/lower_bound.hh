#pragma once

#include "cc/bzd/type_traits/iterator/advance.hh"
#include "cc/bzd/type_traits/iterator/distance.hh"
#include "cc/bzd/type_traits/iterator/traits.hh"
#include "cc/bzd/type_traits/range/traits.hh"
#include "cc/bzd/utility/comparison/less.hh"
#include "cc/bzd/utility/forward.hh"

namespace bzd::algorithm {

/// Returns an iterator pointing to the first element in the range [first, last) that is not less than (i.e. greater or equal to) value,
/// or last if no such element is found.
///
/// \param[in] first The beginning iterator defining the partially-ordered range to examine.
/// \param[in] last The ending iterator defining the partially-ordered range to examine.
/// \param[in] value The value to compare the elements to.
/// \param[in] comparison Binary predicate which returns ​true if the first argument is less than (i.e. is ordered before) the second.
///
/// \return Iterator pointing to the first element that is not less than value, or last if no such element is found.
template <class Iterator, class T, class Compare = bzd::Less<typename typeTraits::Iterator<Iterator>::ValueType>>
requires concepts::forwardIterator<Iterator>
constexpr Iterator lowerBound(Iterator first, Iterator last, const T& value, Compare comparison = Compare{})
{
	using DifferenceType = typename bzd::typeTraits::Iterator<Iterator>::DifferenceType;

	Iterator it;
	DifferenceType count = bzd::distance(first, last);
	DifferenceType step{};

	while (count > 0)
	{
		it = first;
		step = count / 2;
		bzd::advance(it, step);
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

/// \copydoc lowerBound
/// \param[in,out] range The partially-ordered range to examine.
template <class Range, class... Args>
requires concepts::forwardRange<Range>
constexpr auto lowerBound(Range range, Args&&... args)
{
	return lowerBound(bzd::begin(range), bzd::end(range), bzd::forward<Args>(args)...);
}

} // namespace bzd::algorithm
