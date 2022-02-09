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
requires concepts::forwardIterator<Iterator>
[[nodiscard]] constexpr bzd::BoolType anyOf(Iterator first, Iterator last, UnaryPredicate predicate) noexcept
{
	return (bzd::algorithm::findIf(first, last, predicate) != last);
}
} // namespace bzd::algorithm
