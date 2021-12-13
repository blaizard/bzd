#pragma once

#include "cc/bzd/algorithm/find_if.hh"

namespace bzd::algorithm {

/// Returns an iterator to the first element in the range [first, last) for which predicate \c predicate returns \c false.
///
/// \param[in] first The beginning of the range of elements to examine.
/// \param[in] last The ending of the range of elements to examine.
/// \param[in] predicate The unary predicate which returns \c false for the required element.
template <class Iterator, class UnaryPredicate>
[[nodiscard]] constexpr Iterator find_if_not(Iterator first, Iterator last, UnaryPredicate predicate) noexcept
{
	return bzd::algorithm::find_if(first, last, !predicate);
}
} // namespace bzd::algorithm
