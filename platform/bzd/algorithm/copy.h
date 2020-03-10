#pragma once

namespace bzd { namespace algorithm {
template <class InputIt, class OutputIt>
constexpr OutputIt copy(InputIt first, InputIt last, OutputIt dFirst)
{
	while (first != last)
	{
		*dFirst++ = *first++;
	}
	return dFirst;
}
}} // namespace bzd::algorithm
