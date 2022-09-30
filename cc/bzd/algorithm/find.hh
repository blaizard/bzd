#pragma once

#include "cc/bzd/algorithm/find_if.hh"
#include "cc/bzd/utility/forward.hh"

namespace bzd::algorithm {

/// Returns an iterator to the first element in the range [first, last) that equals to value.
///
/// \param[in] first The beginning of the range of elements to examine.
/// \param[in] last The ending of the range of elements to examine.
/// \param[in] value The value to be searched.
template <class Iterator, class T>
requires concepts::forwardIterator<Iterator>
[[nodiscard]] constexpr Iterator find(Iterator first, Iterator last, const T& value) noexcept
{
	return bzd::algorithm::findIf(first, last, [&value](const auto& item) { return item == value; });
}

/// \copydoc find
/// \param[in,out] range The range of elements to examine.
template <class Range, class... Args>
requires concepts::forwardRange<Range>
constexpr auto find(Range&& range, Args&&... args) { return find(bzd::begin(range), bzd::end(range), bzd::forward<Args>(args)...); }

} // namespace bzd::algorithm
