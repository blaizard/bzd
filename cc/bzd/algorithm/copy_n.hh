#pragma once

#include "cc/bzd/platform/types.hh"
#include "cc/bzd/type_traits/is_iterator.hh"

namespace bzd::algorithm {
template <class InputIt, class OutputIt>
constexpr OutputIt copyN(InputIt first, const SizeType size, OutputIt result)
{
	static_assert(typeTraits::isIterator<InputIt> && typeTraits::isIterator<OutputIt>, "Only iterators can be used with copyN.");
	static_assert(iterator::isCategory<InputIt, iterator::ForwardTag>, "The input iterator must be a forward iterator.");
	static_assert(iterator::isCategory<OutputIt, iterator::ForwardTag>, "The output iterator must be a forward iterator.");

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
