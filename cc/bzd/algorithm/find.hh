#pragma once

#include "cc/bzd/algorithm/find_if.hh"
#include "cc/bzd/type_traits/sentinel_for.hh"
#include "cc/bzd/utility/forward.hh"

namespace bzd::algorithm {

/// Returns an iterator to the first element in the range [first, last) that equals to value.
///
/// \param[in] first The beginning of the range of elements to examine.
/// \param[in] last The ending of the range of elements to examine.
/// \param[in] value The value to be searched.
template <concepts::forwardIterator Iterator, concepts::sentinelFor<Iterator> Sentinel, class T>
[[nodiscard]] constexpr Iterator find(Iterator first, Sentinel last, const T& value) noexcept
{
	return bzd::algorithm::findIf(first, last, [&value](const auto& item) { return item == value; });
}

/// \copydoc find
/// \param[in,out] range The range of elements to examine.
template <concepts::forwardRange Range, class... Args>
constexpr auto find(Range&& range, Args&&... args)
{
	return bzd::algorithm::find(bzd::begin(range), bzd::end(range), bzd::forward<Args>(args)...);
}

} // namespace bzd::algorithm
