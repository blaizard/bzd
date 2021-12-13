#pragma once

#include "cc/bzd/type_traits/is_iterator.hh"

namespace bzd::algorithm {

/// Returns an iterator to the first element in the range [first, last) for which predicate \c predicate returns \c true.
///
/// \param[in] first The beginning of the range of elements to examine.
/// \param[in] last The ending of the range of elements to examine.
/// \param[in] predicate The unary predicate which returns \c ​true for the required element.
template <class Iterator, class UnaryPredicate>
[[nodiscard]] constexpr Iterator find_if(Iterator first, Iterator last, UnaryPredicate predicate) noexcept
{
	static_assert(typeTraits::isIterator<Iterator>, "Only iterators can be used with find_if.");
	static_assert(iterator::isCategory<Iterator, iterator::ForwardTag>, "The iterator must be a forward iterator.");

	for (; first != last; ++first)
	{
		if (predicate(*first))
		{
			return first;
		}
	}
	return last;
}
} // namespace bzd::algorithm
