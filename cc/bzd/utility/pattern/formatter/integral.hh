#pragma once

#include "cc/bzd/algorithm/reverse.hh"
#include "cc/bzd/container/array.hh"
#include "cc/bzd/container/optional.hh"
#include "cc/bzd/container/range/views/reverse.hh"
#include "cc/bzd/container/string.hh"
#include "cc/bzd/container/vector.hh"
#include "cc/bzd/meta/always_false.hh"
#include "cc/bzd/type_traits/container.hh"
#include "cc/bzd/type_traits/is_floating_point.hh"
#include "cc/bzd/type_traits/is_integral.hh"
#include "cc/bzd/type_traits/is_signed.hh"

namespace bzd::format::impl {
namespace {
static constexpr bzd::Array<const char, 16> digits{inPlace, '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};
}

template <Size base = 10, concepts::appendableWithBytes Container, class T, class Digits>
constexpr bzd::Optional<bzd::Size> integer(Container& str, const T& n, const Digits& digits = bzd::format::impl::digits) noexcept
{
	static_assert(base > 1 && base <= 16, "Invalid base size.");
	static_assert(Digits::size() >= base, "There is not enough digits for the base.");
	static_assert(sizeof(T) <= 8, "Only up to 64-bit integers are supported.");

	bzd::String<64u> buffer{};

	T number = n;
	if constexpr (bzd::typeTraits::isSigned<T>)
	{
		number = (n < 0) ? -n : n;
	}

	do
	{
		const auto digit = digits[static_cast<Size>(number % base)];
		number /= base;
		if (!buffer.append(digit))
		{
			return bzd::nullopt;
		}
	} while (number);

	if constexpr (bzd::typeTraits::isSigned<T>)
	{
		if (n < 0)
		{
			if (!buffer.append('-'))
			{
				return bzd::nullopt;
			}
		}
	}

	for (const auto c : buffer | bzd::range::reverse())
	{
		str.append(c);
	}
	return buffer.size();
}

template <concepts::appendableWithBytes Container, class T>
constexpr bzd::Optional<bzd::Size> floatingPoint(Container& str, const T& n, const Size maxPrecision) noexcept
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

	const auto result = bzd::format::impl::integer(str, static_cast<int>(roundedNumber), bzd::format::impl::digits);
	if (!result)
	{
		return bzd::nullopt;
	}

	if (!str.append('.'))
	{
		return bzd::nullopt;
	}

	auto counter = result.value() + 1u;
	auto afterComma = (roundedNumber - static_cast<T>(static_cast<int>(n)));
	while (afterComma >= resolution * 2)
	{
		afterComma *= 10;
		resolution *= 10;

		const auto digit = static_cast<int>(afterComma);
		if (!str.append('0' + digit))
		{
			return bzd::nullopt;
		}
		++counter;
		afterComma -= static_cast<T>(digit);
	}

	return counter;
}
} // namespace bzd::format::impl

namespace bzd::format {

template <concepts::appendableWithBytes Container, concepts::integral T>
constexpr Optional<Size> toStringHex(Container& str,
									 const T& data,
									 const bzd::Array<const char, 16>& digits = bzd::format::impl::digits) noexcept
{
	return bzd::format::impl::integer<16u>(str, data, digits);
}

template <concepts::appendableWithBytes Container, concepts::integral T>
constexpr Optional<Size> toStringOct(Container& str, const T& data) noexcept
{
	return bzd::format::impl::integer<8u>(str, data, bzd::format::impl::digits);
}

template <concepts::appendableWithBytes Container, concepts::integral T>
constexpr Optional<Size> toStringBin(Container& str, const T& data) noexcept
{
	return bzd::format::impl::integer<2u>(str, data, bzd::format::impl::digits);
}
} // namespace bzd::format

// Integers

template <bzd::concepts::appendableWithBytes Container, bzd::concepts::integral T>
constexpr bzd::Optional<bzd::Size> toString(Container& str, const T data) noexcept
{
	return bzd::format::impl::integer(str, data, bzd::format::impl::digits);
}

// Floating points

template <bzd::concepts::appendableWithBytes Container, bzd::concepts::floatingPoint T>
constexpr bzd::Optional<bzd::Size> toString(Container& str, const T data, const bzd::Size maxPrecision = 6) noexcept
{
	return bzd::format::impl::floatingPoint(str, data, maxPrecision);
}

// Boolean

template <bzd::concepts::appendableWithBytes Container>
constexpr bzd::Optional<bzd::Size> toString(Container& str, const bzd::Bool value) noexcept
{
	return str.append((value) ? "true"_sv.asBytes() : "false"_sv.asBytes());
}

// Chars

template <bzd::concepts::appendableWithBytes Container>
constexpr bzd::Optional<bzd::Size> toString(Container& str, const char c) noexcept
{
	return str.append(c);
}
