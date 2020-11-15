
#pragma once

namespace bzd::algorithm {
template <class InputIt, class T>
constexpr void fill(InputIt first, InputIt last, const T& value)
{
    for (; first != last; ++first)
	{
        *first = value;
    }
}
} // namespace bzd::algorithm
