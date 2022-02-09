#pragma once

#include "cc/bzd/type_traits/is_iterator.hh"
#include "cc/bzd/type_traits/range.hh"

namespace bzd::algorithm {

/// Copies the elements from the range, defined by [first, last), to another range ending at \c result.
/// The elements are copied in reverse order (the last element is copied first), but their relative order is preserved.
///
/// \param[in] first The beginning of the range of elements to copy from.
/// \param[in] last The ending of the range of elements to copy from.
/// \param[out] result The end of the destination range.
///
/// \return Output iterator to the element in the destination range, one past the last element copied.
template <class InputIt, class OutputIt>
requires concepts::bidirectionalIterator<InputIt> && concepts::bidirectionalIterator<OutputIt>
constexpr OutputIt copyBackward(InputIt first, InputIt last, OutputIt result) noexcept
{
	while (first != last)
	{
		*(--result) = *(--last);
	}
	return result;
}

/// \copydoc copyBackward
/// \param[in] input The range of elements to copy from.
/// \param[out] output The range of the destination range.
template <class InputRange, class OutputRange>
requires concepts::range<InputRange> && concepts::range<OutputRange>
constexpr auto copyBackward(InputRange input, OutputRange output) noexcept
{
	return copyBackward(bzd::begin(input), bzd::end(input), bzd::end(output));
}

} // namespace bzd::algorithm
