#pragma once

#include "cc/bzd/container/array.hh"
#include "cc/bzd/container/string_view.hh"
#include "cc/bzd/type_traits/is_integral.hh"
#include "cc/bzd/type_traits/is_signed.hh"
#include "cc/bzd/utility/begin.hh"
#include "cc/bzd/utility/end.hh"

namespace bzd::reader::impl {

namespace {
static constexpr bzd::Array<const char, 16> digits{inPlace, '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};
}

/// Read an integer from a string starting from the begining.
/// The integer string representation does not have to be the same size as the string.
template <class Range, Size base = 10, class T, class Digits>
constexpr Size integer(Range&& range, T& n, const Digits& digits = bzd::reader::impl::digits) noexcept
{
	static_assert(base > 1 && base <= 16, "Invalid base size.");
	static_assert(Digits::size() >= base, "There is not enough digits for the base.");

	n = 0;
	Size index = 0u;
	auto it = bzd::begin(range);
	auto last = bzd::end(range);

	Bool isNegative = false;
	if (*it == '-')
	{
		if (!concepts::isSigned<T>)
		{
			return 0u;
		}
		isNegative = true;
		++it;
	}

	while (!(it == last))
	{
		const Size value = *it - digits[0];
		if (value >= base)
		{
			break;
		}
		n = n * base + value;
		++it;
		++index;
	}

	if (isNegative)
	{
		n = -n;
	}

	if (index)
	{
		return index + ((isNegative) ? 1u : 0u);
	}
	return 0u;
}

} // namespace bzd::reader::impl

template <bzd::concepts::integral T>
constexpr bzd::Size fromString(const bzd::StringView& str, T& data) noexcept
{
	return bzd::reader::impl::integer(str, data, bzd::reader::impl::digits);
}
