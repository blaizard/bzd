#pragma once

#include "cc/bzd/algorithm/find_if.hh"
#include "cc/bzd/type_traits/iterator.hh"
#include "cc/bzd/type_traits/range.hh"
#include "cc/bzd/utility/forward.hh"

namespace bzd::algorithm {

/// Checks if unary predicate \c predicate returns \c true for no elements in the range [first, last).
///
/// \param[in] first The beginning of the range of elements to examine.
/// \param[in] last The ending of the range of elements to examine.
/// \param[in] predicate The unary predicate.
template <class Iterator, class UnaryPredicate>
requires concepts::forwardIterator<Iterator>
[[nodiscard]] constexpr bzd::BoolType noneOf(Iterator first, Iterator last, UnaryPredicate predicate) noexcept
{
	return (bzd::algorithm::findIf(first, last, predicate) == last);
}

/// \copydoc noneOf
/// \param[in] range The range of elements to examine.
template <class Range, class... Args>
requires concepts::forwardRange<Range>
[[nodiscard]] constexpr auto noneOf(Range&& range, Args&&... args) noexcept
{
	return noneOf(bzd::begin(range), bzd::end(range), bzd::forward<Args>(args)...);
}

} // namespace bzd::algorithm
