#pragma once

namespace bzd::algorithm {
template <class InputIt, class OutputIt>
constexpr OutputIt copy(InputIt first, InputIt last, OutputIt result) noexcept
{
	while (first != last)
	{
		*result++ = *first++;
	}
	return result;
}
} // namespace bzd::algorithm
