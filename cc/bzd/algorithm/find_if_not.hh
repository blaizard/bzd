#pragma once

#include "cc/bzd/algorithm/find_if.hh"
#include "cc/bzd/utility/forward.hh"

namespace bzd::algorithm {

/// Returns an iterator to the first element in the range [first, last) for which predicate \c predicate returns \c false.
///
/// \param[in] first The beginning of the range of elements to examine.
/// \param[in] last The ending of the range of elements to examine.
/// \param[in] predicate The unary predicate which returns \c false for the required element.
template <class Iterator, class UnaryPredicate>
requires concepts::forwardIterator<Iterator>
[[nodiscard]] constexpr Iterator findIfNot(Iterator first, Iterator last, UnaryPredicate predicate) noexcept
{
	return bzd::algorithm::findIf(first, last, !predicate);
}

/// \copydoc findIfNot
/// \param[in,out] range The range of elements to examine.
template <class Range, class... Args>
requires concepts::forwardRange<Range>
constexpr auto findIfNot(Range&& range, Args&&... args)
{
	return findIfNot(bzd::begin(range), bzd::end(range), bzd::forward<Args>(args)...);
}

} // namespace bzd::algorithm
