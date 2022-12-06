#pragma once

#include "cc/bzd/type_traits/iterator.hh"
#include "cc/bzd/type_traits/predicate.hh"
#include "cc/bzd/type_traits/range.hh"
#include "cc/bzd/type_traits/sentinel_for.hh"
#include "cc/bzd/utility/advance.hh"
#include "cc/bzd/utility/comparison/less.hh"
#include "cc/bzd/utility/distance.hh"
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
template <concepts::forwardIterator Iterator,
		  concepts::sentinelFor<Iterator> Sentinel,
		  class T,
		  concepts::predicate<typeTraits::IteratorValue<Iterator>, T> Compare = bzd::Less<typeTraits::IteratorValue<Iterator>>>
constexpr Iterator lowerBound(Iterator first, Sentinel last, const T& value, Compare comparison = Compare{})
{
	Iterator it{first};
	typeTraits::IteratorDifference<Iterator> count = bzd::distance(first, last);
	typeTraits::IteratorDifference<Iterator> step{};

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
template <concepts::forwardRange Range, class... Args>
constexpr auto lowerBound(Range&& range, Args&&... args)
{
	return bzd::algorithm::lowerBound(bzd::begin(range), bzd::end(range), bzd::forward<Args>(args)...);
}

} // namespace bzd::algorithm
