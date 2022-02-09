#pragma once

#include "cc/bzd/platform/types.hh"
#include "cc/bzd/type_traits/is_iterator.hh"

namespace bzd::algorithm {

/// Copies exactly \c count values from the range beginning at first to the range beginning at result.
///
/// \param[in] first The beginning of the range of elements to copy from.
/// \param[in] count The number of the elements to copy.
/// \param[out] result The beginning of the destination range.
///
/// \return Iterator in the destination range, pointing past the last element copied if count>0 or result otherwise.
template <class InputIt, class OutputIt>
requires concepts::forwardIterator<InputIt> && concepts::forwardIterator<OutputIt>
constexpr OutputIt copyN(InputIt first, const SizeType count, OutputIt result)
{
	if (count > 0)
	{
		*result++ = *first;
		for (SizeType i = 1; i < count; ++i)
		{
			*result++ = *++first;
		}
	}
	return result;
}
} // namespace bzd::algorithm
