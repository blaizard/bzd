#pragma once

#include "cc/bzd/algorithm/find_if_not.hh"
#include "cc/bzd/type_traits/iterator.hh"
#include "cc/bzd/type_traits/predicate.hh"
#include "cc/bzd/type_traits/range.hh"
#include "cc/bzd/type_traits/sentinel_for.hh"

namespace bzd::algorithm {

/// Checks if unary predicate \c predicate returns \c true for at least one element in the range [first, last).
///
/// \param[in] first The beginning of the range of elements to examine.
/// \param[in] last The ending of the range of elements to examine.
/// \param[in] predicate The unary predicate.
template <concepts::forwardIterator Iterator,
		  concepts::sentinelFor<Iterator> Sentinel,
		  concepts::predicate<typeTraits::IteratorValue<Iterator>> UnaryPredicate>
[[nodiscard]] constexpr bzd::Bool anyOf(Iterator first, Sentinel last, UnaryPredicate predicate) noexcept
{
	return (bzd::algorithm::findIf(first, last, predicate) != last);
}

/// \copydoc anyOf
/// \param[in] range The range of elements to examine.
template <concepts::forwardRange Range, concepts::predicate<typeTraits::RangeValue<Range>> UnaryPredicate>
[[nodiscard]] constexpr auto anyOf(Range&& range, UnaryPredicate predicate) noexcept
{
	return bzd::algorithm::anyOf(bzd::begin(range), bzd::end(range), predicate);
}

} // namespace bzd::algorithm
