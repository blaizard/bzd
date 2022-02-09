#pragma once

#include "cc/bzd/container/iterator/advance.hh"
#include "cc/bzd/container/iterator/distance.hh"
#include "cc/bzd/type_traits/is_iterator.hh"
#include "cc/bzd/type_traits/range.hh"
#include "cc/bzd/utility/comparison/less.hh"
#include "cc/bzd/utility/forward.hh"

namespace bzd::algorithm {

/// Returns an iterator pointing to the first element in the range [first, last) that is greater than value
/// or last if no such element is found.
///
/// \param[in] first The beginning iterator defining the partially-ordered range to examine.
/// \param[in] last The ending iterator defining the partially-ordered range to examine.
/// \param[in] value The value to compare the elements to.
/// \param[in] comparison Binary predicate which returns ​true if the first argument is less than (i.e. is ordered before) the second.
///
/// \return iterator pointing to the first element that is greater than value, or last if no such element is found.
template <class Iterator, class T, class Compare = bzd::Less<typename iterator::Traits<Iterator>::ValueType>>
requires concepts::forwardIterator<Iterator>
constexpr Iterator upperBound(Iterator first, Iterator last, const T& value, Compare comparison = Compare{})
{
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

/// \copydoc upperBound
/// \param[in,out] range The partially-ordered range to examine.
template <class Range, class... Args>
requires concepts::range<Range>
constexpr auto upperBound(Range range, Args&&... args)
{
	return upperBound(bzd::begin(range), bzd::end(range), bzd::forward<Args>(args)...);
}

} // namespace bzd::algorithm
