#pragma once

#include "cc/bzd/type_traits/is_iterator.hh"

namespace bzd::algorithm {

/// Assigns the given value to the elements in the range [first, last).
///
/// \param[in,out] first The beginning of the range of elements to modify.
/// \param[in,out] last The ending of the range of elements to modify.
/// \param[in] value The value to be assigned.
template <class Iterator, class T>
requires concepts::forwardIterator<Iterator>
constexpr void fill(Iterator first, Iterator last, const T& value)
{
	for (; first != last; ++first)
	{
		*first = value;
	}
}
} // namespace bzd::algorithm
