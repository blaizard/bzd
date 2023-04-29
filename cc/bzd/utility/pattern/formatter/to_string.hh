#pragma once

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
		hexadecimal_lower,
		hexadecimal_upper,
		fixed_point,
		fixed_point_percent,
		pointer
	};

	bzd::Size index = 0;
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
	static constexpr bzd::Optional<Size> process(Range& range, const T& value, [[maybe_unused]] const Metadata& metadata) noexcept
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

class SchemaFormat
{
public:
	using Metadata = bzd::format::impl::Metadata;

	/// \brief Parse a metadata conversion string.
	///
	/// Format compatible with python format (with some exceptions)
	///
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
	/// p    Pointer
	/// %	Percentage. Multiples by 100 and puts % at the end.
	template <class Adapter>
	static constexpr void parse(Metadata& metadata, bzd::StringView& format) noexcept
	{
		// Parse sign: [sign]
		parseSign<Adapter>(format, metadata);
		Adapter::assertTrue(format.size() > 0, "Replacement field format ended abruptly (after parseSign)");

		// Parse alternate form [#]
		if (format.front() == '#')
		{
			metadata.alternate = true;
			format.removePrefix(1);
			Adapter::assertTrue(format.size() > 0, "Replacement field format ended abruptly '#')");
		}

		// Parse precision [.precision]
		if (format.front() == '.')
		{
			format.removePrefix(1);
			metadata.isPrecision = parseUnsignedInteger(format, metadata.precision);
			Adapter::assertTrue(format.size() > 0, "Replacement field format ended abruptly (after precision)");
		}

		// Parse type [type]
		if (format.front() != '}')
		{
			switch (format.front())
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
				metadata.format = Metadata::Format::hexadecimal_lower;
				break;
			case 'X':
				metadata.format = Metadata::Format::hexadecimal_upper;
				break;
			case 'f':
				metadata.format = Metadata::Format::fixed_point;
				break;
			case '%':
				metadata.format = Metadata::Format::fixed_point_percent;
				break;
			case 'p':
				metadata.format = Metadata::Format::pointer;
				break;
			default:
				Adapter::onError("Unsupported conversion format, only the following is "
								 "supported: [bdoxXfp%]");
			}
			format.removePrefix(1);
			Adapter::assertTrue(format.size() > 0, "Replacement field format ended abruptly (after type)");
		}
		Adapter::assertTrue(format.front() == '}', "Invalid format for replacement field, expecting '}'");

		format.removePrefix(1);
	}

	template <class Adapter, class ValueType>
	static constexpr void check(const Metadata& metadata) noexcept
	{
		switch (metadata.format)
		{
		case Metadata::Format::binary:
		case Metadata::Format::octal:
		case Metadata::Format::hexadecimal_lower:
		case Metadata::Format::hexadecimal_upper:
			Adapter::assertTrue(bzd::typeTraits::isIntegral<ValueType>, "Argument must be an integral");
			break;
		case Metadata::Format::decimal:
		case Metadata::Format::fixed_point:
		case Metadata::Format::fixed_point_percent:
			Adapter::assertTrue(bzd::typeTraits::isArithmetic<ValueType>, "Argument must be arithmetic");
			break;
		case Metadata::Format::pointer:
			[[fallthrough]];
		case Metadata::Format::automatic:
			break;
		}
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

	// Sign

	template <class Adapter>
	static constexpr void parseSign(bzd::StringView& format, Metadata& metadata) noexcept
	{
		switch (format.front())
		{
		case '+':
			metadata.sign = Metadata::Sign::always;
			break;
		case '-':
			metadata.sign = Metadata::Sign::only_negative;
			break;
		}
		if (metadata.sign != Metadata::Sign::automatic)
		{
			format.removePrefix(1);
		}
	}
};

// ---- toString specializations ----

constexpr bzd::StringView processCommon(const bzd::StringView stringView, const Metadata& metadata) noexcept
{
	switch (metadata.format)
	{
	case Metadata::Format::automatic:
		return ((metadata.isPrecision) ? stringView.subStr(0, bzd::min(metadata.precision, stringView.size())) : stringView);
	case Metadata::Format::fixed_point:
	case Metadata::Format::fixed_point_percent:
	case Metadata::Format::decimal:
	case Metadata::Format::binary:
	case Metadata::Format::hexadecimal_lower:
	case Metadata::Format::hexadecimal_upper:
	case Metadata::Format::octal:
	case Metadata::Format::pointer:
		break;
	}
	return {};
}

template <concepts::integral T>
constexpr bzd::Optional<bzd::Size> toString(bzd::interface::String& str, const T value, const Metadata& metadata) noexcept
{
	switch (metadata.format)
	{
	case Metadata::Format::automatic:
	case Metadata::Format::decimal:
		return ::toString(str, value);
	case Metadata::Format::binary:
		if (metadata.alternate)
		{
			str += "0b"_sv;
		}
		return bzd::format::toStringBin(str, value);
	case Metadata::Format::hexadecimal_lower:
		if (metadata.alternate)
		{
			str += "0x"_sv;
		}
		return bzd::format::toStringHex(str, value);
	case Metadata::Format::hexadecimal_upper:
		if (metadata.alternate)
		{
			str += "0x"_sv;
		}
		{
			constexpr bzd::Array<const char, 16>
				digits{inPlace, '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
			return bzd::format::toStringHex(str, value, digits);
		}
	case Metadata::Format::octal:
		if (metadata.alternate)
		{
			str += "0o"_sv;
		}
		return bzd::format::toStringOct(str, value);
	case Metadata::Format::fixed_point:
	case Metadata::Format::fixed_point_percent:
	case Metadata::Format::pointer:
		break;
	}
	return bzd::nullopt;
}

template <concepts::floatingPoint T>
constexpr bzd::Optional<bzd::Size> toString(bzd::interface::String& str, const T value, const Metadata& metadata) noexcept
{
	switch (metadata.format)
	{
	case Metadata::Format::automatic:
	case Metadata::Format::decimal:
		return ::toString(str, value);
	case Metadata::Format::fixed_point:
		return ::toString(str, value, (metadata.isPrecision) ? metadata.precision : 6);
	case Metadata::Format::fixed_point_percent:
	{
		const auto result = ::toString(str, value * 100., (metadata.isPrecision) ? metadata.precision : 6);
		if (result)
		{
			if (str.append("%"_sv) == 1u)
			{
				return result.value() + 1u;
			}
		}
	}
	break;
	case Metadata::Format::binary:
	case Metadata::Format::hexadecimal_lower:
	case Metadata::Format::hexadecimal_upper:
	case Metadata::Format::octal:
	case Metadata::Format::pointer:
		break;
	}
	return bzd::nullopt;
}

template <class T>
requires(concepts::pointer<T> && !concepts::constructible<bzd::StringView, T>)
constexpr bzd::Optional<bzd::Size> toString(bzd::interface::String& str, const T value, const Metadata&) noexcept
{
	Metadata metadata{};
	metadata.format = Metadata::Format::hexadecimal_lower;
	metadata.alternate = true;
	return toString(str, reinterpret_cast<Size>(value), metadata);
}

constexpr bzd::Optional<bzd::Size> toString(bzd::interface::String& str,
											const bzd::StringView stringView,
											const Metadata& metadata) noexcept
{
	const auto data = processCommon(stringView, metadata);
	if (str.append(data) == data.size())
	{
		return data.size();
	}
	return bzd::nullopt;
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
/// p    Pointer
/// %	Percentage. Multiples by 100 and puts % at the end.
/// \endcode
///
/// This is an after text
///
/// \param out Output stream where the formating string will be written to.
/// \param str run-time or compile-time string containing the format.
/// \param args Arguments to be passed for the format.
template <bzd::concepts::appendableWithBytes Container, bzd::concepts::constexprStringView Pattern, class... Args>
constexpr bzd::Optional<bzd::Size> toString(Container& str, const Pattern& pattern, const Args&... args) noexcept
{
	const auto [parser, processor] =
		bzd::pattern::impl::make<Container&, bzd::format::impl::StringFormatter, bzd::format::impl::SchemaFormat>(pattern, args...);
	bzd::Size count{0u};

	// Run-time call
	for (const auto& result : parser)
	{
		if (!result.str.empty())
		{
			if (str.append(result.str) != result.str.size())
			{
				return bzd::nullopt;
			}
			count += result.str.size();
		}
		if (result.isMetadata)
		{
			const auto maybeCount = processor.process(str, result.metadata);
			if (!maybeCount)
			{
				return bzd::nullopt;
			}
			count += maybeCount.value();
		}
	}

	return count;
}
