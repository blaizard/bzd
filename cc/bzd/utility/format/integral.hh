#pragma once

#include "cc/bzd/algorithm/reverse.hh"
#include "cc/bzd/container/array.hh"
#include "cc/bzd/container/string.hh"
#include "cc/bzd/container/vector.hh"
#include "cc/bzd/meta/always_false.hh"
#include "cc/bzd/type_traits/is_floating_point.hh"
#include "cc/bzd/type_traits/is_integral.hh"
#include "cc/bzd/type_traits/is_signed.hh"

namespace bzd::format::impl {
namespace {
static constexpr bzd::Array<const char, 16> digits{inPlace, '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};
}

template <Size base = 10, class T, class U>
constexpr void integer(interface::String& str, const T& n, U& digits = bzd::format::impl::digits) noexcept
{
	static_assert(base > 1 && base <= 16, "Invalid base size.");
	static_assert(U::size() >= base, "There is not enough digits for the base.");

	auto data = str.data();
	const Size indexBegin = str.size();
	Size index = indexBegin;
	const Size indexEnd = str.capacity();
	T number = n;
	if constexpr (bzd::typeTraits::isSigned<T>)
	{
		number = (n < 0) ? -n : n;
	}

	if (index != indexEnd)
	{
		do
		{
			const auto digit = digits[static_cast<Size>(number % base)];
			number /= base;
			data[index++] = digit;
		} while (number && index != indexEnd);

		if constexpr (bzd::typeTraits::isSigned<T>)
		{
			if (n < 0 && index != indexEnd)
			{
				data[index++] = '-';
			}
		}
	}
	str.resize(index);
	// NOLINTNEXTLINE(bugprone-narrowing-conversions)
	bzd::algorithm::reverse(str.begin() + indexBegin, str.end());
}

template <class T>
constexpr void floatingPoint(interface::String& str, const T& n, const Size maxPrecision) noexcept
{
	constexpr const T resolutionList[15] = {1,
											0.1,
											0.01,
											0.001,
											0.0001,
											0.00001,
											0.000001,
											0.0000001,
											0.00000001,
											0.000000001,
											0.0000000001,
											0.00000000001,
											0.000000000001,
											0.0000000000001,
											0.00000000000001};

	auto resolution = resolutionList[maxPrecision];
	const T roundedNumber = n + resolution / 2;

	bzd::format::impl::integer(str, static_cast<int>(roundedNumber), bzd::format::impl::digits);

	str += '.';

	auto afterComma = (roundedNumber - static_cast<T>(static_cast<int>(n)));
	while (afterComma >= resolution * 2)
	{
		afterComma *= 10;
		resolution *= 10;

		const auto digit = static_cast<int>(afterComma);
		str.append('0' + digit);
		afterComma -= static_cast<T>(digit);
	}
}
} // namespace bzd::format::impl

namespace bzd::format {

template <class T, bzd::typeTraits::EnableIf<bzd::typeTraits::isIntegral<T>, void>* = nullptr>
constexpr void toStringHex(bzd::interface::String& str,
						   const T& data,
						   const bzd::Array<const char, 16>& digits = bzd::format::impl::digits) noexcept
{
	bzd::format::impl::integer<16>(str, data, digits);
}

template <class T, bzd::typeTraits::EnableIf<bzd::typeTraits::isIntegral<T>, T>* = nullptr>
constexpr void toStringOct(bzd::interface::String& str, const T& data) noexcept
{
	bzd::format::impl::integer<8>(str, data, bzd::format::impl::digits);
}

template <class T, bzd::typeTraits::EnableIf<bzd::typeTraits::isIntegral<T>, T>* = nullptr>
constexpr void toStringBin(bzd::interface::String& str, const T& data) noexcept
{
	bzd::format::impl::integer<2>(str, data, bzd::format::impl::digits);
}
} // namespace bzd::format

// Integers

template <class T>
requires bzd::concepts::integral<T>
constexpr void toString(bzd::interface::String& str, const T data) noexcept
{
	bzd::format::impl::integer(str, data, bzd::format::impl::digits);
}

// Floating points

template <class T>
requires bzd::concepts::floatingPoint<T>
constexpr void toString(bzd::interface::String& str, const T data, const bzd::Size maxPrecision = 6) noexcept
{
	bzd::format::impl::floatingPoint(str, data, maxPrecision);
}

// Boolean

constexpr void toString(bzd::interface::String& str, const bzd::Bool value) noexcept
{
	str.append((value) ? "true"_sv : "false"_sv);
}

// Chars

constexpr void toString(bzd::interface::String& str, const char c) noexcept
{
	str.append(c);
}
