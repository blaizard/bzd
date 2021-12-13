#pragma once

#include "cc/bzd/algorithm/find_if.hh"

namespace bzd::algorithm {

/// Returns an iterator to the first element in the range [first, last) that equals to value.
///
/// \param[in] first The beginning of the range of elements to examine.
/// \param[in] last The ending of the range of elements to examine.
/// \param[in] value The value to be searched.
template <class Iterator, class T>
[[nodiscard]] constexpr Iterator find(Iterator first, Iterator last, const T& value) noexcept
{
	return bzd::algorithm::find_if(first, last, [&value](const auto& item) { return item == value; });
}
} // namespace bzd::algorithm
