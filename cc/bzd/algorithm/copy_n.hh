#pragma once

#include "cc/bzd/platform/types.hh"

namespace bzd::algorithm {
template <class InputIt, class OutputIt>
constexpr OutputIt copyN(InputIt first, const SizeType size, OutputIt result)
{
	if (size > 0)
	{
		*result++ = *first;
		for (SizeType i = 1; i < size; ++i)
		{
			*result++ = *++first;
		}
	}
	return result;
}
} // namespace bzd::algorithm
