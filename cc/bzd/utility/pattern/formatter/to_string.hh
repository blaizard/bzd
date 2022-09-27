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
		AUTO,
		ALWAYS,
		ONLY_NEGATIVE
	};

	enum class Format
	{
		AUTO,
		BINARY,
		DECIMAL,
		OCTAL,
		HEXADECIMAL_LOWER,
		HEXADECIMAL_UPPER,
		FIXED_POINT,
		FIXED_POINT_PERCENT,
		POINTER
	};

	bzd::Size index = 0;
	Sign sign = Sign::AUTO;
	bool alternate = false;
	bool isPrecision = false;
	bzd::Size precision = 0;
	Format format = Format::AUTO;
};

// ---- Formatter ----

template <class Range, class T>
concept toStringFormatterWithMetadata = requires(Range range, T value)
{
	toString(range, value, bzd::typeTraits::declval<const Metadata>());
};

class StringFormatter
{
public:
	template <class Range, class T>
	static constexpr void process(Range& range, const T& value, [[maybe_unused]] const Metadata& metadata) noexcept
	{
		if constexpr (toStringFormatterWithMetadata<Range, T>)
		{
			toString(range, value, metadata);
		}
		else
		{
			toString(range, value);
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
				metadata.format = Metadata::Format::BINARY;
				break;
			case 'd':
				metadata.format = Metadata::Format::DECIMAL;
				break;
			case 'o':
				metadata.format = Metadata::Format::OCTAL;
				break;
			case 'x':
				metadata.format = Metadata::Format::HEXADECIMAL_LOWER;
				break;
			case 'X':
				metadata.format = Metadata::Format::HEXADECIMAL_UPPER;
				break;
			case 'f':
				metadata.format = Metadata::Format::FIXED_POINT;
				break;
			case '%':
				metadata.format = Metadata::Format::FIXED_POINT_PERCENT;
				break;
			case 'p':
				metadata.format = Metadata::Format::POINTER;
				break;
			default:
				Adapter::onError(
					"Unsupported conversion format, only the following is "
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
		case Metadata::Format::BINARY:
		case Metadata::Format::OCTAL:
		case Metadata::Format::HEXADECIMAL_LOWER:
		case Metadata::Format::HEXADECIMAL_UPPER:
			Adapter::assertTrue(bzd::typeTraits::isIntegral<ValueType>, "Argument must be an integral");
			break;
		case Metadata::Format::DECIMAL:
		case Metadata::Format::FIXED_POINT:
		case Metadata::Format::FIXED_POINT_PERCENT:
			Adapter::assertTrue(bzd::typeTraits::isArithmetic<ValueType>, "Argument must be arithmetic");
			break;
		case Metadata::Format::POINTER:
			[[fallthrough]];
		case Metadata::Format::AUTO:
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
			metadata.sign = Metadata::Sign::ALWAYS;
			break;
		case '-':
			metadata.sign = Metadata::Sign::ONLY_NEGATIVE;
			break;
		}
		if (metadata.sign != Metadata::Sign::AUTO)
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
	case Metadata::Format::AUTO:
		return ((metadata.isPrecision) ? stringView.subStr(0, bzd::min(metadata.precision, stringView.size())) : stringView);
	case Metadata::Format::FIXED_POINT:
	case Metadata::Format::FIXED_POINT_PERCENT:
	case Metadata::Format::DECIMAL:
	case Metadata::Format::BINARY:
	case Metadata::Format::HEXADECIMAL_LOWER:
	case Metadata::Format::HEXADECIMAL_UPPER:
	case Metadata::Format::OCTAL:
	case Metadata::Format::POINTER:
		break;
	}
	return {};
}

template <concepts::integral T>
constexpr void toString(bzd::interface::String& str, const T value, const Metadata& metadata) noexcept
{
	switch (metadata.format)
	{
	case Metadata::Format::AUTO:
	case Metadata::Format::DECIMAL:
		::toString(str, value);
		break;
	case Metadata::Format::BINARY:
		if (metadata.alternate)
		{
			str += "0b"_sv;
		}
		bzd::format::toStringBin(str, value);
		break;
	case Metadata::Format::HEXADECIMAL_LOWER:
		if (metadata.alternate)
		{
			str += "0x"_sv;
		}
		bzd::format::toStringHex(str, value);
		break;
	case Metadata::Format::HEXADECIMAL_UPPER:
		if (metadata.alternate)
		{
			str += "0x"_sv;
		}
		{
			constexpr bzd::Array<const char, 16>
				digits{inPlace, '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
			bzd::format::toStringHex(str, value, digits);
		}
		break;
	case Metadata::Format::OCTAL:
		if (metadata.alternate)
		{
			str += "0o"_sv;
		}
		bzd::format::toStringOct(str, value);
		break;
	case Metadata::Format::FIXED_POINT:
	case Metadata::Format::FIXED_POINT_PERCENT:
	case Metadata::Format::POINTER:
		break;
	}
}

template <concepts::floatingPoint T>
constexpr void toString(bzd::interface::String& str, const T value, const Metadata& metadata) noexcept
{
	switch (metadata.format)
	{
	case Metadata::Format::AUTO:
	case Metadata::Format::DECIMAL:
		::toString(str, value);
		break;
	case Metadata::Format::FIXED_POINT:
		::toString(str, value, (metadata.isPrecision) ? metadata.precision : 6);
		break;
	case Metadata::Format::FIXED_POINT_PERCENT:
		::toString(str, value * 100., (metadata.isPrecision) ? metadata.precision : 6);
		str += "%"_sv;
		break;
	case Metadata::Format::BINARY:
	case Metadata::Format::HEXADECIMAL_LOWER:
	case Metadata::Format::HEXADECIMAL_UPPER:
	case Metadata::Format::OCTAL:
	case Metadata::Format::POINTER:
		break;
	}
}

template <class T>
requires(concepts::pointer<T> && !concepts::constructible<bzd::StringView, T>) constexpr void toString(bzd::interface::String& str,
																									   const T value,
																									   const Metadata&) noexcept
{
	Metadata metadata{};
	metadata.format = Metadata::Format::HEXADECIMAL_LOWER;
	metadata.alternate = true;
	toString(str, reinterpret_cast<Size>(value), metadata);
}

constexpr void toString(bzd::interface::String& str, const bzd::StringView stringView, const Metadata& metadata) noexcept
{
	str += processCommon(stringView, metadata);
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
template <bzd::concepts::containerToString Container, bzd::concepts::constexprStringView Pattern, class... Args>
constexpr void toString(Container& str, const Pattern& pattern, const Args&... args) noexcept
{
	const auto [parser, processor] =
		bzd::pattern::impl::make<Container&, bzd::format::impl::StringFormatter, bzd::format::impl::SchemaFormat>(pattern, args...);

	// Run-time call
	for (const auto& result : parser)
	{
		if (!result.str.empty())
		{
			str.append(result.str);
		}
		if (result.metadata.hasValue())
		{
			processor.process(str, result.metadata.value());
		}
	}
}
