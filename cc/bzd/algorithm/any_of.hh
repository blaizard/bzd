#pragma once

#include "cc/bzd/algorithm/find_if_not.hh"
#include "cc/bzd/type_traits/is_iterator.hh"

namespace bzd::algorithm {

/// Checks if unary predicate \c predicate returns \c true for at least one element in the range [first, last).
///
/// \param[in] first The beginning of the range of elements to examine.
/// \param[in] last The ending of the range of elements to examine.
/// \param[in] predicate The unary predicate.
template <class Iterator, class UnaryPredicate>
[[nodiscard]] constexpr bzd::BoolType any_of(Iterator first, Iterator last, UnaryPredicate predicate) noexcept
{
	static_assert(typeTraits::isIterator<Iterator>, "Only iterators can be used with all_of.");
	static_assert(iterator::isCategory<Iterator, iterator::ForwardTag>, "The iterator must be a forward iterator.");

	return (bzd::algorithm::find_if(first, last, predicate) != last);
}
} // namespace bzd::algorithm
