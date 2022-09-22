#pragma once

#include "cc/bzd/platform/types.hh"
#include "cc/bzd/type_traits/iterator.hh"
#include "cc/bzd/type_traits/range.hh"

namespace bzd::algorithm {

/// Copies exactly \c count values from the range beginning at first to the range beginning at result.
///
/// \param[in] first The beginning of the range of elements to copy from.
/// \param[in] count The number of the elements to copy.
/// \param[out] result The beginning of the destination range.
///
/// \return Iterator in the destination range, pointing past the last element copied if count>0 or result otherwise.
template <class InputIt, class OutputIt>
requires concepts::forwardIterator<InputIt> && concepts::outputIterator<OutputIt>
constexpr OutputIt copyN(InputIt first, const Size count, OutputIt result)
{
	if (count > 0)
	{
		*result++ = *first;
		for (Size i = 1; i < count; ++i)
		{
			*result++ = *++first;
		}
	}
	return result;
}

/// \copydoc copy
/// \param[in] input The range of elements to copy from.
/// \param[out] output The range of the destination range.
template <class InputRange, class OutputRange>
requires concepts::forwardRange<InputRange> && concepts::outputRange<OutputRange>
constexpr auto copyN(InputRange&& input, const Size count, OutputRange&& output)
{
	return copyN(bzd::begin(input), count, bzd::begin(output));
}

} // namespace bzd::algorithm
