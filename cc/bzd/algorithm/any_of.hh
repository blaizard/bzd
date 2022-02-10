#pragma once

#include "cc/bzd/algorithm/find_if_not.hh"
#include "cc/bzd/type_traits/iterator/traits.hh"
#include "cc/bzd/type_traits/range/traits.hh"

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

/// \copydoc anyOf
/// \param[in] range The range of elements to examine.
template <class Range, class UnaryPredicate>
requires concepts::forwardRange<Range>
[[nodiscard]] constexpr auto anyOf(Range range, UnaryPredicate predicate) noexcept
{
	return anyOf(bzd::begin(range), bzd::end(range), predicate);
}

} // namespace bzd::algorithm
