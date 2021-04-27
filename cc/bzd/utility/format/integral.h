#pragma once

#include "bzd/algorithm/reverse.h"
#include "bzd/container/string.h"
#include "bzd/container/string_channel.h"
#include "bzd/container/vector.h"
#include "bzd/core/channel.h"
#include "bzd/type_traits/is_floating_point.h"
#include "bzd/type_traits/is_integral.h"

namespace bzd::format::impl {
namespace {
static constexpr const char* const digits = "0123456789abcdef";
}

template <SizeType Base = 10, class T, bzd::typeTraits::EnableIf<(Base > 1 && Base <= 16), T>* = nullptr>
constexpr void integer(interface::String& str, const T& n, const char* const digits = bzd::format::impl::digits)
{
	auto data = str.data();
	SizeType index = 0;
	const SizeType indexEnd = str.capacity();
	T number = (n < 0) ? -n : n;

	if (index != indexEnd)
	{
		do
		{
			const int digit = digits[static_cast<int>(number % Base)];
			number /= Base;
			data[index++] = static_cast<char>(digit);
		} while (number && index != indexEnd);

		if (n < 0 && index != indexEnd)
		{
			data[index++] = '-';
		}
	}
	str.resize(index);
	bzd::algorithm::reverse(str.begin(), str.end());
}

template <class T>
constexpr void fixedPoint(bzd::OChannel& stream, const T& n, const SizeType maxPrecision) noexcept
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

	bzd::String<40> buffer;
	bzd::format::impl::integer(buffer, static_cast<int>(roundedNumber));
	stream.write(buffer.asBytes());

	stream.write(bzd::StringView{".", 1}.asBytes());

	buffer.clear();
	auto afterComma = (roundedNumber - static_cast<T>(static_cast<int>(n)));
	while (afterComma >= resolution * 2)
	{
		afterComma *= 10;
		resolution *= 10;

		const auto digit = static_cast<int>(afterComma);
		buffer.append('0' + digit);
		afterComma -= static_cast<T>(digit);
	}
	stream.write(buffer.asBytes());
}
} // namespace bzd::format::impl

namespace bzd::format {
template <class T, bzd::typeTraits::EnableIf<typeTraits::isIntegral<T>, T>* = nullptr>
constexpr void toString(bzd::OChannel& stream, const T& data)
{
	bzd::String<40> buffer; // 40 is a the length
	bzd::format::impl::integer(buffer, data);
	stream.write(buffer.asBytes());
}

template <class T, bzd::typeTraits::EnableIf<typeTraits::isFloatingPoint<T>, void>* = nullptr>
constexpr void toString(bzd::OChannel& stream, const T& data, const SizeType maxPrecision = 6)
{
	bzd::format::impl::fixedPoint(stream, data, maxPrecision);
}

template <class T, bzd::typeTraits::EnableIf<typeTraits::isIntegral<T>, void>* = nullptr>
constexpr void toStringHex(bzd::OChannel& stream, const T& data, const char* const digits = bzd::format::impl::digits)
{
	bzd::String<16> buffer; // 16 is a the length of a 128-bit data in binary
	bzd::format::impl::integer<16>(buffer, data, digits);
	stream.write(buffer.asBytes());
}

template <class T, bzd::typeTraits::EnableIf<typeTraits::isIntegral<T>, T>* = nullptr>
constexpr void toStringOct(bzd::OChannel& stream, const T& data)
{
	bzd::String<32> buffer;
	bzd::format::impl::integer<8>(buffer, data);
	stream.write(buffer.asBytes());
}

template <class T, bzd::typeTraits::EnableIf<typeTraits::isIntegral<T>, T>* = nullptr>
constexpr void toStringBin(bzd::OChannel& stream, const T& data)
{
	bzd::Vector<bzd::UInt16Type, 4> shortList;
	T number = data;
	while (number)
	{
		shortList.pushBack(static_cast<bzd::UInt16Type>(number & 0xffff));
		number >>= 16;
	}

	bzd::String<16> buffer;
	for (int i = static_cast<int>(shortList.size()) - 1; i >= 0; --i)
	{
		bzd::format::impl::integer<2>(buffer, shortList[i]);
		if (i < static_cast<int>(shortList.size()) - 1)
		{
			// Fill the string with zeros
			const auto n = 16 - buffer.size();
			bzd::String<16> padding(n, '0');
			stream.write(padding.asBytes());
		}
		stream.write(buffer.asBytes());
	}
}

void toString(bzd::OChannel& stream, const bzd::StringView& data);

void toString(bzd::OChannel& stream, const char c);

template <class... Args>
constexpr void appendToString(bzd::interface::String& str, Args&&... args)
{
	bzd::interface::StringChannel stream(str);
	toString(static_cast<bzd::OChannel&>(stream), bzd::forward<Args>(args)...);
}

template <class... Args>
constexpr void toString(bzd::interface::String& str, Args&&... args)
{
	str.clear();
	appendToString(str, bzd::forward<Args>(args)...);
}
} // namespace bzd::format
