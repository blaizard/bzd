#pragma once

#include "cc/bzd/type_traits/is_iterator.hh"

namespace bzd::algorithm {

/// Copies all elements in the range [first, last) starting from first and proceeding to last - 1.
///
/// \param[in] first The beginning of the range of elements to copy from.
/// \param[in] last The ending of the range of elements to copy from.
/// \param[out] result The beginning of the destination range.
///
/// \return Output iterator to the element in the destination range, one past the last element copied.
template <class InputIt, class OutputIt>
requires concepts::forwardIterator<InputIt> && concepts::forwardIterator<OutputIt>
constexpr OutputIt copy(InputIt first, InputIt last, OutputIt result) noexcept
{
	while (first != last)
	{
		*result++ = *first++;
	}
	return result;
}
} // namespace bzd::algorithm
