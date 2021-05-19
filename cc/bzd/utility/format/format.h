#pragma once

#include "bzd/container/string.h"
#include "bzd/container/string_view.h"
#include "bzd/container/tuple.h"
#include "bzd/container/vector.h"
#include "bzd/core/assert/minimal.h"
#include "bzd/core/channel.h"
#include "bzd/meta/range.h"
#include "bzd/type_traits/decay.h"
#include "bzd/type_traits/declval.h"
#include "bzd/type_traits/is_arithmetic.h"
#include "bzd/type_traits/is_constructible.h"
#include "bzd/type_traits/is_floating_point.h"
#include "bzd/type_traits/is_integral.h"
#include "bzd/type_traits/is_pointer.h"
#include "bzd/utility/format/integral.h"

#include <iostream>

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

	bzd::SizeType index = 0;
	Sign sign = Sign::AUTO;
	bool alternate = false;
	bool isPrecision = false;
	bzd::SizeType precision = 0;
	Format format = Format::AUTO;
};

template <class T>
class HasFormatter
{
	template <class C, class = decltype(toString(bzd::typeTraits::declval<bzd::OChannel&>(), bzd::typeTraits::declval<C>()))>
	static bzd::typeTraits::TrueType test(bzd::SizeType);
	template <class C>
	static bzd::typeTraits::FalseType test(...);

public:
	static constexpr bool value = decltype(test<T>(0))::value;
};

template <class T>
class HasFormatterWithMetadata
{
	template <class C,
			  class = decltype(toString(
				  bzd::typeTraits::declval<bzd::OChannel&>(), bzd::typeTraits::declval<C>(), bzd::typeTraits::declval<const Metadata>()))>
	static bzd::typeTraits::TrueType test(bzd::SizeType);
	template <class C>
	static bzd::typeTraits::FalseType test(...);

public:
	static constexpr bool value = decltype(test<T>(0))::value;
};

/**
 * Type removal of the type-specific formatter.
 */
class Formatter
{
public:
	virtual void print(bzd::OChannel& os, const Metadata& metadata) const {};
};

template <class T>
class FormatterSpecialized : public Formatter
{
public:
	explicit constexpr FormatterSpecialized(const T& v) : Formatter{}, value_{v} {}

	template <class U = T, bzd::typeTraits::EnableIf<!HasFormatterWithMetadata<U>::value, void>* = nullptr>
	void printToString(bzd::OChannel& os, const Metadata& /*metadata*/) const
	{
		toString(os, value_);
	}

	template <class U = T, bzd::typeTraits::EnableIf<HasFormatterWithMetadata<U>::value, void>* = nullptr>
	void printToString(bzd::OChannel& os, const Metadata& metadata) const
	{
		toString(os, value_, metadata);
	}

	void print(bzd::OChannel& os, const Metadata& metadata) const override { printToString(os, metadata); }

private:
	// Can be optimized to avoid copy if non-temporary
	const T value_;
};

/**
 * Parse an unsigned integer
 */
static constexpr bool parseUnsignedInteger(bzd::StringView& format, bzd::SizeType& integer)
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

/**
 * Return the positional index
 */
template <class Ctx>
constexpr bzd::SizeType parseIndex(Ctx& context, bzd::StringView& format, const bzd::SizeType autoIndex)
{
	bzd::SizeType index = 0;
	const bool isDefined = parseUnsignedInteger(format, index);
	if (format.front() == ':')
	{
		format.removePrefix(1);
	}
	else
	{
		context.assertTrue(format.front() == '}', "Expecting closing '}' for the replacement field");
	}
	return (isDefined) ? index : autoIndex;
}

// Sign

template <class Ctx>
constexpr void parseSign(Ctx& context, bzd::StringView& format, Metadata& metadata)
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

/**
 * \brief Parse a metadata conversion string.
 *
 * Format compatible with python format (with some exceptions)
 *
 * format_spec ::=  [sign][#][.precision][type]
 * sign        ::=  "+" | "-" | " "
 * precision   ::=  integer
 * type        ::=  "b" | "d" | "f" | "o" | "x" | "X" | "f" | "p" | "%"
 * d	Decimal integer
 * b	Binary format
 * o	Octal format
 * x	Hexadecimal format (lower case)
 * X	Hexadecimal format (upper case)
 * f	Displays fixed point number (Default: 6)
 * p    Pointer
 * %	Percentage. Multiples by 100 and puts % at the end.
 */
template <class Ctx>
constexpr Metadata parseMetadata(Ctx& context, bzd::StringView& format, const bzd::SizeType current)
{
	const auto endIndex = format.find('}');
	context.assertTrue(endIndex != bzd::StringView::npos, "Missing closing '}' for the replacement field");
	auto metadataStr = format.subStr(0, endIndex + 1);

	Metadata metadata;

	// Look for the index
	metadata.index = parseIndex(context, metadataStr, current);
	context.assertTrue(metadataStr.size() > 0, "Replacement field format ended abruptly (after parseIndex)");

	// Parse sign: [sign]
	parseSign(context, metadataStr, metadata);
	context.assertTrue(metadataStr.size() > 0, "Replacement field format ended abruptly (after parseSign)");

	// Parse alternate form [#]
	if (metadataStr.front() == '#')
	{
		metadata.alternate = true;
		metadataStr.removePrefix(1);
		context.assertTrue(metadataStr.size() > 0, "Replacement field format ended abruptly '#')");
	}

	// Parse precision [.precision]
	if (metadataStr.front() == '.')
	{
		metadataStr.removePrefix(1);
		metadata.isPrecision = parseUnsignedInteger(metadataStr, metadata.precision);
		context.assertTrue(metadataStr.size() > 0, "Replacement field format ended abruptly (after precision)");
	}

	// Parse type [type]
	if (metadataStr.front() != '}')
	{
		switch (metadataStr.front())
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
			context.onError(
				"Unsupported conversion format, only the following is "
				"supported: [bdoxXfp%]");
		}
		metadataStr.removePrefix(1);
		context.assertTrue(metadataStr.size() > 0, "Replacement field format ended abruptly (after type)");
	}
	context.assertTrue(metadataStr.front() == '}', "Invalid format for replacement field, expecting '}'");

	format.removePrefix(endIndex + 1);
	return metadata;
}

struct ResultStaticString
{
	bool isMetadata;
	StringView str;
};

/**
 * Parse a static string and returns when there is data to be processed.
 */
template <class Ctx>
constexpr ResultStaticString parseStaticString(Ctx& context, bzd::StringView& format)
{
	SizeType offset = 0;
	while (offset < format.size())
	{
		const auto c = format[offset++];
		if (c != '{' && c != '}')
		{
			continue;
		}

		context.assertTrue(offset < format.size(), "Unexpected static string termination");

		if (format[offset] == c)
		{
			const auto str = format.subStr(0, offset);
			format.removePrefix(offset + 1);
			return {false, str};
		}

		context.assertTrue(c == '{', "'}' must be part of a pair '{...}' or doubled '}}'");

		--offset;
		break;
	}

	const auto str = format.subStr(0, offset);
	format.removePrefix(offset);
	return {!format.empty(), str};
}

template <class Ctx, class T>
constexpr void parse(Ctx& context, bzd::StringView format, const T& args)
{
	bzd::SizeType autoIndex = 0;
	do
	{
		const auto result = parseStaticString(context, format);
		if (!result.str.empty())
		{
			context.processSubstring(result.str);
		}
		if (result.isMetadata)
		{
			context.assertTrue(format.front() == '{', "Unexpected return state for parseStaticString");
			context.assertTrue(format.size() > 1, "Unexpected return state for parseStaticString");
			format.removePrefix(1);
			const auto metadata = parseMetadata(context, format, autoIndex++);
			context.assertTrue(metadata.index < args.size(),
							   "The index specified is greater than the number of "
							   "arguments provided");
			context.processMetadata(metadata);
		}
	} while (!format.empty());
}

/**
 * Context used for the current parsing operation.
 * Different context are used to check or print the formated string.
 */
template <class T>
class Context : public T
{
public:
	using T::T;
	constexpr inline void assertTrue(const bool condition, const bzd::StringView& message) const
	{
		if (!condition)
		{
			T::onError(message);
		}
	}
};

class CheckContext : public bzd::VectorConstexpr<Metadata, 128>
{
public:
	constexpr CheckContext() = default;
	constexpr void processSubstring(const bzd::StringView&) {}
	constexpr void processMetadata(const Metadata& metadata) { pushBack(metadata); }
	constexpr void onError(const bzd::StringView& message) const { bzd::assert::isTrueConstexpr(false); }
};

template <class T>
void printInteger(bzd::OChannel& stream, const T& value, const Metadata& metadata)
{
	switch (metadata.format)
	{
	case Metadata::Format::AUTO:
	case Metadata::Format::DECIMAL:
		bzd::format::toString(stream, value);
		break;
	case Metadata::Format::BINARY:
		if (metadata.alternate)
		{
			stream.write(bzd::StringView{"0b"}.asBytes());
		}
		bzd::format::toStringBin(stream, value);
		break;
	case Metadata::Format::HEXADECIMAL_LOWER:
		if (metadata.alternate)
		{
			stream.write(bzd::StringView{"0x"}.asBytes());
		}
		bzd::format::toStringHex(stream, value);
		break;
	case Metadata::Format::HEXADECIMAL_UPPER:
		if (metadata.alternate)
		{
			stream.write(bzd::StringView{"0x"}.asBytes());
		}
		bzd::format::toStringHex(stream, value, "0123456789ABCDEF");
		break;
	case Metadata::Format::OCTAL:
		if (metadata.alternate)
		{
			stream.write(bzd::StringView{"0o"}.asBytes());
		}
		bzd::format::toStringOct(stream, value);
		break;
	case Metadata::Format::FIXED_POINT:
	case Metadata::Format::FIXED_POINT_PERCENT:
	case Metadata::Format::POINTER:
		break;
	}
}

template <class T>
void printFixedPoint(bzd::OChannel& stream, const T& value, const Metadata& metadata)
{
	switch (metadata.format)
	{
	case Metadata::Format::AUTO:
	case Metadata::Format::DECIMAL:
		bzd::format::toString(stream, value);
		break;
	case Metadata::Format::FIXED_POINT:
		bzd::format::toString(stream, value, (metadata.isPrecision) ? metadata.precision : 6);
		break;
	case Metadata::Format::FIXED_POINT_PERCENT:
		bzd::format::toString(stream, value * 100., (metadata.isPrecision) ? metadata.precision : 6);
		stream.write(bzd::StringView("%").asBytes());
		break;
	case Metadata::Format::BINARY:
	case Metadata::Format::HEXADECIMAL_LOWER:
	case Metadata::Format::HEXADECIMAL_UPPER:
	case Metadata::Format::OCTAL:
	case Metadata::Format::POINTER:
		break;
	}
}

// Specialization of toString for the native types

template <class T, bzd::typeTraits::EnableIf<bzd::typeTraits::isIntegral<T>, void>* = nullptr>
void toString(bzd::OChannel& stream, const T& value, const Metadata& metadata)
{
	printInteger(stream, value, metadata);
}

template <class T, bzd::typeTraits::EnableIf<bzd::typeTraits::isFloatingPoint<T>, void>* = nullptr>
void toString(bzd::OChannel& stream, const T& value, const Metadata& metadata)
{
	printFixedPoint(stream, value, metadata);
}

template <
	class T,
	bzd::typeTraits::EnableIf<bzd::typeTraits::isPointer<T> && !bzd::typeTraits::isConstructible<bzd::StringView, T>, void>* = nullptr>
void toString(bzd::OChannel& stream, const T& value, const Metadata&)
{
	Metadata metadata{};
	metadata.format = Metadata::Format::HEXADECIMAL_LOWER;
	metadata.alternate = true;
	printInteger(stream, reinterpret_cast<SizeType>(value), metadata);
}

static void toString(bzd::OChannel& stream, const bzd::StringView stringView, const Metadata& metadata)
{
	switch (metadata.format)
	{
	case Metadata::Format::AUTO:
		stream.write(
			((metadata.isPrecision) ? stringView.subStr(0, bzd::min(metadata.precision, stringView.size())) : stringView).asBytes());
		break;
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
}

class PrintContext
{
public:
	constexpr PrintContext(bzd::OChannel& stream, const bzd::interface::Vector<const bzd::format::impl::Formatter*>& args) :
		stream_(stream), args_(args)
	{
	}
	void processSubstring(const bzd::StringView& str) { stream_.write(str.asBytes()); }
	void processMetadata(const Metadata& metadata) { args_[metadata.index]->print(stream_, metadata); }
	constexpr void onError(const bzd::StringView& message) const {}

private:
	bzd::OChannel& stream_;
	const bzd::interface::Vector<const bzd::format::impl::Formatter*>& args_;
};

template <class T>
constexpr Context<CheckContext> contextBuild(const bzd::StringView& format, const T& tuple)
{
	Context<CheckContext> ctx{};
	parse(ctx, format, tuple);
	return ctx;
}

static void print(bzd::OChannel& stream,
				  const bzd::StringView& format,
				  const bzd::interface::Vector<const bzd::format::impl::Formatter*>& args)
{
	Context<PrintContext> ctx(stream, args);
	parse(ctx, format, args);
}

/**
 * \brief Check the format context.
 *
 * Check the format context with the argument type, this to ensure type safety.
 * This function should only be used at compile time.
 */
template <SizeType N, class Ctx, class T, bzd::typeTraits::EnableIf<(N > 0)>* = nullptr>
constexpr bool contextCheck(const Ctx& context, const T& tuple)
{
	auto value = tuple.template get<N - 1>();
	context.assertTrue(HasFormatter<decltype(value)>::value || HasFormatterWithMetadata<decltype(value)>::value,
					   "Argument type is not supported, it must contain a valid formatter.");

	bool usedAtLeastOnce = false;
	for (const auto& metadata : context)
	{
		if (metadata.index == N - 1)
		{
			usedAtLeastOnce = true;
			switch (metadata.format)
			{
			case Metadata::Format::BINARY:
			case Metadata::Format::OCTAL:
			case Metadata::Format::HEXADECIMAL_LOWER:
			case Metadata::Format::HEXADECIMAL_UPPER:
				context.assertTrue(bzd::typeTraits::isIntegral<decltype(value)>, "Argument must an integral");
				break;
			case Metadata::Format::DECIMAL:
			case Metadata::Format::FIXED_POINT:
			case Metadata::Format::FIXED_POINT_PERCENT:
				context.assertTrue(bzd::typeTraits::isArithmetic<decltype(value)>, "Argument must arithmetic");
				break;
			case Metadata::Format::POINTER:
				break;
			case Metadata::Format::AUTO:
				break;
			}
		}
	}

	context.assertTrue(usedAtLeastOnce, "At least one argument is not being used by the formating string");
	return contextCheck<N - 1>(context, tuple);
}

template <SizeType N, class Ctx, class T, bzd::typeTraits::EnableIf<(N == 0)>* = nullptr>
constexpr bool contextCheck(const Ctx&, const T&)
{
	return true;
}

template <SizeType... I, class... Args>
constexpr void toStringRuntime(bzd::OChannel& out, const bzd::StringView& str, bzd::meta::range::Type<I...>, Args&&... args)
{
	using bzd::format::impl::FormatterSpecialized;

	// Run-time call
	const bzd::Tuple<FormatterSpecialized<bzd::typeTraits::Decay<Args>>...> tuple{
		FormatterSpecialized<bzd::typeTraits::Decay<Args>>(args)...};
	const bzd::Vector<const bzd::format::impl::Formatter*, sizeof...(args)> argList{(&tuple.template get<I>())...};
	bzd::format::impl::print(out, str, argList);
}

} // namespace bzd::format::impl

namespace bzd::format {
/**
 * \brief String formating.
 *
 * Lightweight and compilation time checking string formating utility.
 * The syntax is compatible with Python format with some limitations.
 *
 * \code
 * format_spec ::=  [sign][#][.precision][type]
 * sign        ::=  "+" | "-" | " "
 * precision   ::=  integer
 * type        ::=  "b" | "d" | "f" | "o" | "x" | "X" | "f" | "p" | "%"
 * d	Decimal integer
 * b	Binary format
 * o	Octal format
 * x	Hexadecimal format (lower case)
 * X	Hexadecimal format (upper case)
 * f	Displays fixed point number (Default: 6)
 * p    Pointer
 * %	Percentage. Multiples by 100 and puts % at the end.
 * \endcode
 *
 * This is an after text
 *
 * \param out Output stream where the formating string will be written to.
 * \param str run-time or compile-time string containing the format.
 * \param args Arguments to be passed for the format.
 */

template <class... Args>
constexpr void toString(bzd::OChannel& out, const bzd::StringView& str, Args&&... args)
{
	// Run-time call
	impl::toStringRuntime(out, str, bzd::meta::Range<0, sizeof...(Args)>{}, bzd::forward<Args>(args)...);
}

template <class ConstexprStringView, class... Args>
constexpr void toString(bzd::OChannel& out, const ConstexprStringView& str, Args&&... args)
{
	// Compile-time format check
	constexpr const bzd::Tuple<bzd::typeTraits::Decay<Args>...> tuple{};
	constexpr const auto context = bzd::format::impl::contextBuild(ConstexprStringView::value(), tuple);
	// This line enforces compilation time evaluation
	static_assert(bzd::format::impl::contextCheck<tuple.size()>(context, tuple), "String format check failed");

	// Run-time call
	bzd::format::toString(out, ConstexprStringView::value(), bzd::forward<Args>(args)...);
}

} // namespace bzd::format
