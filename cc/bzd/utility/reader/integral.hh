#pragma once

#include "cc/bzd/container/array.hh"
#include "cc/bzd/container/optional.hh"
#include "cc/bzd/container/string_view.hh"
#include "cc/bzd/type_traits/is_integral.hh"
#include "cc/bzd/type_traits/is_signed.hh"
#include "cc/bzd/utility/begin.hh"
#include "cc/bzd/utility/end.hh"

namespace bzd::reader::impl {

namespace {
static constexpr bzd::Array<const char, 16> digits{inPlace, '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};
}

// constexpr Span<const char> integer(const Span<const char> str, T& n, const Digits& digits = bzd::reader::impl::digits) noexcept
// constexpr Optional<StringView> integer(const StringView str, T& n, const Digits& digits = bzd::reader::impl::digits) noexcept
// -> constexpr Optional<Iterator> integer(Iterator first, Iterator last, T& n, const Digits& digits = bzd::reader::impl::digits) noexcept
// This allow to take any kind of container
// To have use range::views (to be implemented)

/// Read an integer from a string starting from the begining.
/// The integer string representation does not have to be the same size as the string.
template <Size base = 10, class Iterator, class T, class Digits>
constexpr bzd::Optional<Iterator> integer(Iterator first, Iterator last, T& n, const Digits& digits = bzd::reader::impl::digits) noexcept
{
	static_assert(base > 1 && base <= 16, "Invalid base size.");
	static_assert(Digits::size() >= base, "There is not enough digits for the base.");

	n = 0;
	Size index = 0u;
	auto it = first;

	Bool isNegative = false;
	if (it != last && *it == '-')
	{
		if (!concepts::isSigned<T>)
		{
			return bzd::nullopt;
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
		return it;
	}
	return bzd::nullopt;
}

template <class Range, Size base = 10, class T, class Digits>
constexpr auto integer(Range&& range, T& n, const Digits& digits = bzd::reader::impl::digits) noexcept
{
	return integer<base>(bzd::begin(range), bzd::end(range), n, digits);
}

template <class Range, concepts::floatingPoint T, char Dot = '.'>
constexpr Size floatingPoint(Range&& range, T& n) noexcept
{
	const auto size = integer(range, n);
	if (size == 0u)
	{
		return 0u;
	}
	return size;
}

} // namespace bzd::reader::impl

/// TODO, this should also work with Spans<const char>
/// and same for toString ...
template <bzd::concepts::integral T>
constexpr auto fromString(const bzd::StringView& str, T& data) noexcept
{
	return bzd::reader::impl::integer(str, data, bzd::reader::impl::digits);
}

// Proposed syntax:
// - Matches the string, unless:
//    - it starts with `[` -> then it matches one of chars.
//    - it starts with `(` -> then it matches one of the sequence.
//    - it starts with `{` -> then it assign the match to a variable.
//    - `+` means one or more.
//    - `*` means 0 or more.
//    - `.` means any character.
// For example:
// - "HTTP{:f}\s+{:i}\s+{[A-Z]*}"
