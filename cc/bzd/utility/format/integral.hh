#pragma once

#include "cc/bzd/algorithm/reverse.hh"
#include "cc/bzd/container/string.hh"
#include "cc/bzd/container/vector.hh"
#include "cc/bzd/type_traits/is_floating_point.hh"
#include "cc/bzd/type_traits/is_integral.hh"
#include "cc/bzd/type_traits/is_signed.hh"

namespace bzd::format::impl {
namespace {
static constexpr const char* const digits = "0123456789abcdef";
}

template <SizeType Base = 10, class T, bzd::typeTraits::EnableIf<(Base > 1 && Base <= 16), T>* = nullptr>
constexpr void integer(interface::String& str, const T& n, const char* const digits = bzd::format::impl::digits)
{
	auto data = str.data();
	const SizeType indexBegin = str.size();
	SizeType index = indexBegin;
	const SizeType indexEnd = str.capacity();
	T number = n;
	if constexpr (bzd::typeTraits::isSigned<T>)
	{
		number = (n < 0) ? -n : n;
	}

	if (index != indexEnd)
	{
		do
		{
			const int digit = digits[static_cast<int>(number % Base)];
			number /= Base;
			data[index++] = static_cast<char>(digit);
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
	bzd::algorithm::reverse(str.begin() + indexBegin, str.end());
}

template <class T>
constexpr void fixedPoint(interface::String& str, const T& n, const SizeType maxPrecision) noexcept
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

	bzd::format::impl::integer(str, static_cast<int>(roundedNumber));

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

template <class T, bzd::typeTraits::EnableIf<typeTraits::isIntegral<T>, T>* = nullptr>
constexpr void toString(bzd::interface::String& str, const T& data)
{
	bzd::format::impl::integer(str, data);
}

template <class T, bzd::typeTraits::EnableIf<typeTraits::isFloatingPoint<T>, void>* = nullptr>
constexpr void toString(bzd::interface::String& str, const T& data, const SizeType maxPrecision = 6)
{
	bzd::format::impl::fixedPoint(str, data, maxPrecision);
}

template <class T, bzd::typeTraits::EnableIf<typeTraits::isIntegral<T>, void>* = nullptr>
constexpr void toStringHex(bzd::interface::String& str, const T& data, const char* const digits = bzd::format::impl::digits)
{
	bzd::format::impl::integer<16>(str, data, digits);
}

template <class T, bzd::typeTraits::EnableIf<typeTraits::isIntegral<T>, T>* = nullptr>
constexpr void toStringOct(bzd::interface::String& str, const T& data)
{
	bzd::format::impl::integer<8>(str, data);
}

template <class T, bzd::typeTraits::EnableIf<typeTraits::isIntegral<T>, T>* = nullptr>
constexpr void toStringBin(bzd::interface::String& str, const T& data)
{
	bzd::format::impl::integer<2>(str, data);
}

constexpr void toString(bzd::interface::String& str, const bzd::StringView& data)
{
	str.append(data);
}

constexpr void toString(bzd::interface::String& str, const char c)
{
	str.append(c);
}

} // namespace bzd::format
