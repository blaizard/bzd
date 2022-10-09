#pragma once

#include "cc/bzd/algorithm/find_if.hh"
#include "cc/bzd/type_traits/sentinel_for.hh"
#include "cc/bzd/utility/forward.hh"

namespace bzd::algorithm {

/// Returns an iterator to the first element in the range [first, last) for which predicate \c predicate returns \c false.
///
/// \param[in] first The beginning of the range of elements to examine.
/// \param[in] last The ending of the range of elements to examine.
/// \param[in] predicate The unary predicate which returns \c false for the required element.
template <concepts::forwardIterator Iterator, concepts::sentinelFor<Iterator> Sentinel, class UnaryPredicate>
[[nodiscard]] constexpr Iterator findIfNot(Iterator first, Sentinel last, UnaryPredicate predicate) noexcept
{
	return bzd::algorithm::findIf(first, last, !predicate);
}

/// \copydoc findIfNot
/// \param[in,out] range The range of elements to examine.
template <concepts::forwardRange Range, class... Args>
constexpr auto findIfNot(Range&& range, Args&&... args)
{
	return findIfNot(bzd::begin(range), bzd::end(range), bzd::forward<Args>(args)...);
}

} // namespace bzd::algorithm
