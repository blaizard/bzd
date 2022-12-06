#pragma once

#include "cc/bzd/algorithm/find_if.hh"
#include "cc/bzd/type_traits/iterator.hh"
#include "cc/bzd/type_traits/predicate.hh"
#include "cc/bzd/type_traits/range.hh"
#include "cc/bzd/type_traits/sentinel_for.hh"
#include "cc/bzd/utility/forward.hh"

namespace bzd::algorithm {

/// Checks if unary predicate \c predicate returns \c true for no elements in the range [first, last).
///
/// \param[in] first The beginning of the range of elements to examine.
/// \param[in] last The ending of the range of elements to examine.
/// \param[in] predicate The unary predicate.
template <concepts::forwardIterator Iterator,
		  concepts::sentinelFor<Iterator> Sentinel,
		  concepts::predicate<typeTraits::IteratorValue<Iterator>> UnaryPredicate>
[[nodiscard]] constexpr bzd::Bool noneOf(Iterator first, Sentinel last, UnaryPredicate predicate) noexcept
{
	return (bzd::algorithm::findIf(first, last, predicate) == last);
}

/// \copydoc noneOf
/// \param[in] range The range of elements to examine.
template <concepts::forwardRange Range, class... Args>
[[nodiscard]] constexpr auto noneOf(Range&& range, Args&&... args) noexcept
{
	return bzd::algorithm::noneOf(bzd::begin(range), bzd::end(range), bzd::forward<Args>(args)...);
}

} // namespace bzd::algorithm
