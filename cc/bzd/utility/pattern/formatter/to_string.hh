#pragma once

#include "cc/bzd/algorithm/byte_copy.hh"
#include "cc/bzd/container/array.hh"
#include "cc/bzd/container/string.hh"
#include "cc/bzd/container/string_view.hh"
#include "cc/bzd/meta/tuple.hh"
#include "cc/bzd/type_traits/declval.hh"
#include "cc/bzd/type_traits/is_arithmetic.hh"
#include "cc/bzd/type_traits/is_constructible.hh"
#include "cc/bzd/type_traits/is_floating_point.hh"
#include "cc/bzd/type_traits/is_integral.hh"
#include "cc/bzd/type_traits/is_pointer.hh"
#include "cc/bzd/utility/pattern/formatter/integral.hh"
#include "cc/bzd/utility/pattern/pattern.hh"

namespace bzd::format::impl {

struct Metadata
{
	enum class Sign
	{
		automatic,
		always,
		only_negative
	};

	enum class Format
	{
		automatic,
		binary,
		decimal,
		octal,
		hexadecimalLower,
		hexadecimalUpper,
		fixedPoint,
		fixedPointPercent,
		pointer
	};

	Sign sign = Sign::automatic;
	bool alternate = false;
	bool isPrecision = false;
	bzd::Size precision = 0;
	Format format = Format::automatic;
};

// ---- Formatter ----

template <class Range, class T>
concept toStringFormatterWithMetadata =
	requires(Range range, T value) { toString(range, value, bzd::typeTraits::declval<const Metadata>()); };

class StringFormatter
{
public:
	template <class Range, class T>
	static constexpr bzd::Optional<Size> process(Range& range, const T& value, [[maybe_unused]] const Metadata metadata) noexcept
	{
		if constexpr (toStringFormatterWithMetadata<Range, T>)
		{
			return toString(range, value, metadata);
		}
		else
		{
			return toString(range, value);
		}
	}
};

// ---- Schema ----

class SpecializationTemp
{
public:
	using Metadata = bzd::format::impl::Metadata;
	template <class Adapter>
	static constexpr Metadata parse(bzd::StringView options) noexcept
	{
		Metadata metadata{};

		while (!options.empty())
		{
			const auto c = options.front();
			options.removePrefix(1);
			switch (c)
			{
			// Parse sign: [sign]
			case '+':
				Adapter::assertTrue(metadata.sign == Metadata::Sign::automatic, "Sign option can only be assigned once.");
				metadata.sign = Metadata::Sign::always;
				break;
			case '-':
				Adapter::assertTrue(metadata.sign == Metadata::Sign::automatic, "Sign option can only be assigned once.");
				metadata.sign = Metadata::Sign::only_negative;
				break;
			// Parse precision [.precision]
			case '.':
				Adapter::assertTrue(!metadata.isPrecision, "Precision option can only be assigned once.");
				metadata.isPrecision = parseUnsignedInteger(options, metadata.precision);
				break;
			// Parse alternate form [#]
			case '#':
				Adapter::assertTrue(!metadata.alternate, "Alternate option can only be assigned once.");
				metadata.alternate = true;
				break;
			// Parse type [type]
			default:
				Adapter::assertTrue(metadata.format == Metadata::Format::automatic, "Conversion format option can only be assigned once.");
				switch (c)
				{
				case 'b':
					metadata.format = Metadata::Format::binary;
					break;
				case 'd':
					metadata.format = Metadata::Format::decimal;
					break;
				case 'o':
					metadata.format = Metadata::Format::octal;
					break;
				case 'x':
					metadata.format = Metadata::Format::hexadecimalLower;
					break;
				case 'X':
					metadata.format = Metadata::Format::hexadecimalUpper;
					break;
				case 'f':
					metadata.format = Metadata::Format::fixedPoint;
					break;
				case '%':
					metadata.format = Metadata::Format::fixedPointPercent;
					break;
				case 'p':
					metadata.format = Metadata::Format::pointer;
					break;
				default:
					Adapter::onError("Unsupported option, only the following is supported: [+-#.<precision>bdoxXfp%]");
				}
			}
		}

		return metadata;
	}

private:
	/// Parse an unsigned integer
	static constexpr bool parseUnsignedInteger(bzd::StringView& format, bzd::Size& integer) noexcept
	{
		bool isDefined = false;
		integer = 0;
		for (; format.size() > 0 && format.front() >= '0' && format.front() <= '9';)
		{
			isDefined = true;
			integer = integer * 10 + (format.front() - '0');
			format.removePrefix(1);
		}
		return isDefined;
	}
};

class SchemaFormat
{
public:
	/// Get the specialization associated with a type.
	template <class T>
	using Specialization = SpecializationTemp;
};

// ---- toString specializations ----

constexpr bzd::StringView processCommon(const bzd::StringView stringView, const Metadata& metadata) noexcept
{
	switch (metadata.format)
	{
	case Metadata::Format::automatic:
		return ((metadata.isPrecision) ? stringView.subStr(0, bzd::min(metadata.precision, stringView.size())) : stringView);
	case Metadata::Format::fixedPoint:
	case Metadata::Format::fixedPointPercent:
	case Metadata::Format::decimal:
	case Metadata::Format::binary:
	case Metadata::Format::hexadecimalLower:
	case Metadata::Format::hexadecimalUpper:
	case Metadata::Format::octal:
	case Metadata::Format::pointer:
		break;
	}
	return {};
}

template <concepts::outputStreamRange Range, concepts::integral T>
constexpr bzd::Optional<bzd::Size> toString(Range&& range, const T value, const Metadata& metadata) noexcept
{
	bzd::Size count = 0u;
	switch (metadata.format)
	{
	case Metadata::Format::automatic:
	case Metadata::Format::decimal:
		return ::toString(range, value);
	case Metadata::Format::binary:
		if (metadata.alternate)
		{
			if (bzd::algorithm::byteCopyReturnSize("0b"_sv, range) != 2u)
			{
				return bzd::nullopt;
			}
			count += 2u;
		}
		return bzd::format::toStringBin(range, value).andThen([&count](const auto value) { return value + count; });
	case Metadata::Format::hexadecimalLower:
		if (metadata.alternate)
		{
			if (bzd::algorithm::byteCopyReturnSize("0x"_sv, range) != 2u)
			{
				return bzd::nullopt;
			}
			count += 2u;
		}
		return bzd::format::toStringHex(range, value).andThen([&count](const auto value) { return value + count; });
	case Metadata::Format::hexadecimalUpper:
		if (metadata.alternate)
		{
			if (bzd::algorithm::byteCopyReturnSize("0x"_sv, range) != 2u)
			{
				return bzd::nullopt;
			}
			count += 2u;
		}
		{
			constexpr bzd::Array<const char, 16>
				digits{inPlace, '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
			return bzd::format::toStringHex(range, value, digits).andThen([&count](const auto value) { return value + count; });
		}
	case Metadata::Format::octal:
		if (metadata.alternate)
		{
			if (bzd::algorithm::byteCopyReturnSize("0o"_sv, range) != 2u)
			{
				return bzd::nullopt;
			}
			count += 2u;
		}
		return bzd::format::toStringOct(range, value).andThen([&count](const auto value) { return value + count; });
	case Metadata::Format::fixedPoint:
	case Metadata::Format::fixedPointPercent:
	case Metadata::Format::pointer:
		break;
	}
	return bzd::nullopt;
}

template <concepts::outputStreamRange Range, concepts::floatingPoint T>
constexpr bzd::Optional<bzd::Size> toString(Range&& range, const T value, const Metadata& metadata) noexcept
{
	switch (metadata.format)
	{
	case Metadata::Format::automatic:
	case Metadata::Format::decimal:
		return ::toString(range, value);
	case Metadata::Format::fixedPoint:
		return ::toString(range, value, (metadata.isPrecision) ? metadata.precision : 6);
	case Metadata::Format::fixedPointPercent:
	{
		const auto result = ::toString(range, value * 100., (metadata.isPrecision) ? metadata.precision : 6);
		if (result)
		{
			if (bzd::algorithm::byteCopyReturnSize("%"_sv, range))
			{
				return result.value() + 1u;
			}
		}
	}
	break;
	case Metadata::Format::binary:
	case Metadata::Format::hexadecimalLower:
	case Metadata::Format::hexadecimalUpper:
	case Metadata::Format::octal:
	case Metadata::Format::pointer:
		break;
	}
	return bzd::nullopt;
}

template <concepts::outputStreamRange Range, class T>
requires(concepts::pointer<T> && !concepts::constructible<bzd::StringView, T>)
constexpr bzd::Optional<bzd::Size> toString(Range&& range, const T value, const Metadata&) noexcept
{
	Metadata metadata{};
	metadata.format = Metadata::Format::hexadecimalLower;
	metadata.alternate = true;
	return toString(range, reinterpret_cast<Size>(value), metadata);
}

template <concepts::outputStreamRange Range>
constexpr bzd::Optional<bzd::Size> toString(Range&& range, const bzd::StringView stringView, const Metadata& metadata) noexcept
{
	const auto data = processCommon(stringView, metadata);
	if (bzd::algorithm::byteCopyReturnSize(data, range) != data.size())
	{
		return bzd::nullopt;
	}
	return data.size();
}

} // namespace bzd::format::impl

/// \brief String formating.
///
/// Lightweight and compilation time checking string formating utility.
/// The syntax is compatible with Python format with some limitations.
///
/// \code
/// format_spec ::=  [sign][#][.precision][type]
/// sign        ::=  "+" | "-" | " "
/// precision   ::=  integer
/// type        ::=  "b" | "d" | "f" | "o" | "x" | "X" | "f" | "p" | "%"
/// d	Decimal integer
/// b	Binary format
/// o	Octal format
/// x	Hexadecimal format (lower case)
/// X	Hexadecimal format (upper case)
/// f	Displays fixed point number (Default: 6)
/// p   Pointer
/// %	Percentage. Multiples by 100 and puts % at the end.
/// \endcode
///
/// This is an after text
///
/// \param out Output stream where the formating string will be written to.
/// \param str run-time or compile-time string containing the format.
/// \param args Arguments to be passed for the format.
template <bzd::concepts::outputStreamRange Range, bzd::concepts::constexprStringView Pattern, class... Args>
constexpr bzd::Optional<bzd::Size> toString(Range&& range, const Pattern& pattern, const Args&... args) noexcept
{
	const auto [parser, processor] =
		bzd::pattern::impl::make<Range&, bzd::format::impl::StringFormatter, bzd::format::impl::SchemaFormat>(pattern, args...);
	bzd::Size count{0u};

	// Run-time call
	for (const auto& result : parser)
	{
		if (!result.str.empty())
		{
			if (bzd::algorithm::byteCopyReturnSize(result.str, range) != result.str.size())
			{
				return bzd::nullopt;
			}
			count += result.str.size();
		}
		if (result.isMetadata)
		{
			const auto maybeCount = processor.process(range, result);
			if (!maybeCount)
			{
				return bzd::nullopt;
			}
			count += maybeCount.value();
		}
	}

	return count;
}
