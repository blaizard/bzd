#pragma once

#include "bzd/container/optional.h"
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

template <class Adapter, class T>
class HasFormatter
{
	template <class C, class = typename Adapter::template FormatterType<C>>
	static bzd::typeTraits::TrueType test(bzd::SizeType);
	template <class C>
	static bzd::typeTraits::FalseType test(...);

public:
	static constexpr bool value = decltype(test<T>(0))::value;
};

template <class Adapter, class T>
class HasFormatterWithMetadata
{
	template <class C, class = typename Adapter::template FormatterWithMetadataType<C>>
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

template <class Adapter, class T>
class FormatterSpecialized : public Formatter
{
public:
	explicit constexpr FormatterSpecialized(const T& v) : Formatter{}, value_{v} {}

	template <class U = T, bzd::typeTraits::EnableIf<!HasFormatterWithMetadata<Adapter, U>::value, void>* = nullptr>
	constexpr void printToString(bzd::OChannel& os, const Metadata& /*metadata*/) const
	{
		toStream(os, value_);
	}

	template <class U = T, bzd::typeTraits::EnableIf<HasFormatterWithMetadata<Adapter, U>::value, void>* = nullptr>
	constexpr void printToString(bzd::OChannel& os, const Metadata& metadata) const
	{
		toStream(os, value_, metadata);
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
template <class Adapter>
constexpr bzd::SizeType parseIndex(bzd::StringView& format, const bzd::SizeType autoIndex)
{
	bzd::SizeType index = 0;
	const bool isDefined = parseUnsignedInteger(format, index);
	if (format.front() == ':')
	{
		format.removePrefix(1);
	}
	else
	{
		Adapter::assertTrue(format.front() == '}', "Expecting closing '}' for the replacement field");
	}
	return (isDefined) ? index : autoIndex;
}

// Sign

template <class Adapter>
constexpr void parseSign(bzd::StringView& format, Metadata& metadata)
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
template <class Adapter>
constexpr Metadata parseMetadata(bzd::StringView& format, const bzd::SizeType current)
{
	const auto endIndex = format.find('}');
	Adapter::assertTrue(endIndex != bzd::StringView::npos, "Missing closing '}' for the replacement field");
	auto metadataStr = format.subStr(0, endIndex + 1);

	Metadata metadata;

	// Look for the index
	metadata.index = parseIndex<Adapter>(metadataStr, current);
	Adapter::assertTrue(metadataStr.size() > 0, "Replacement field format ended abruptly (after parseIndex)");

	// Parse sign: [sign]
	parseSign<Adapter>(metadataStr, metadata);
	Adapter::assertTrue(metadataStr.size() > 0, "Replacement field format ended abruptly (after parseSign)");

	// Parse alternate form [#]
	if (metadataStr.front() == '#')
	{
		metadata.alternate = true;
		metadataStr.removePrefix(1);
		Adapter::assertTrue(metadataStr.size() > 0, "Replacement field format ended abruptly '#')");
	}

	// Parse precision [.precision]
	if (metadataStr.front() == '.')
	{
		metadataStr.removePrefix(1);
		metadata.isPrecision = parseUnsignedInteger(metadataStr, metadata.precision);
		Adapter::assertTrue(metadataStr.size() > 0, "Replacement field format ended abruptly (after precision)");
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
			Adapter::onError(
				"Unsupported conversion format, only the following is "
				"supported: [bdoxXfp%]");
		}
		metadataStr.removePrefix(1);
		Adapter::assertTrue(metadataStr.size() > 0, "Replacement field format ended abruptly (after type)");
	}
	Adapter::assertTrue(metadataStr.front() == '}', "Invalid format for replacement field, expecting '}'");

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
template <class Adapter>
constexpr ResultStaticString parseStaticString(bzd::StringView& format)
{
	SizeType offset = 0;
	while (offset < format.size())
	{
		const auto c = format[offset++];
		if (c != '{' && c != '}')
		{
			continue;
		}

		Adapter::assertTrue(offset < format.size(), "Unexpected static string termination");

		if (format[offset] == c)
		{
			const auto str = format.subStr(0, offset);
			format.removePrefix(offset + 1);
			return {false, str};
		}

		Adapter::assertTrue(c == '{', "'}' must be part of a pair '{...}' or doubled '}}'");

		--offset;
		break;
	}

	const auto str = format.subStr(0, offset);
	format.removePrefix(offset);
	return {!format.empty(), str};
}

template <class Adapter>
class Parse
{
public:
	constexpr Parse(bzd::StringView format) noexcept : iteratorBegin_{format} {}

	class Iterator
	{
	public:
		struct Result
		{
			const bzd::Optional<const Metadata&> metadata;
			const StringView& str;
		};

	public:
		constexpr Iterator(bzd::StringView format) noexcept : format_{format} { next(); }

		constexpr Iterator& operator++() noexcept
		{
			next();
			return *this;
		}

		constexpr operator bool() const noexcept { return end_; }

		constexpr Result operator*() const noexcept
		{
			return (result_.isMetadata) ? Result{metadata_, result_.str} : Result{nullopt, result_.str};
		}

	private:
		constexpr void next() noexcept
		{
			if (format_.empty())
			{
				end_ = true;
			}
			else
			{
				result_ = parseStaticString<Adapter>(format_);
				if (result_.isMetadata)
				{
					Adapter::assertTrue(format_.front() == '{', "Unexpected return state for parseStaticString");
					Adapter::assertTrue(format_.size() > 1, "Unexpected return state for parseStaticString");
					format_.removePrefix(1);
					metadata_ = parseMetadata<Adapter>(format_, autoIndex_++);
				}
			}
		}

	private:
		bzd::StringView format_;
		ResultStaticString result_{};
		Metadata metadata_{};
		SizeType autoIndex_ = 0;
		bool end_ = false;
	};

	constexpr Iterator begin() noexcept { return iteratorBegin_; }

	constexpr bool end() noexcept { return true; }

private:
	const Iterator iteratorBegin_;
};

/**
 * Adapter used for the current parsing operation.
 * Different adapters are used for compile timme or runtime operations.
 */
template <class... Ts>
class Adapter : public Ts...
{
public:
	static constexpr void assertTrue(const bool condition, const bzd::StringView& message)
	{
		if (!condition)
		{
			Adapter::onError(message);
		}
	}
};

class ConstexprAssert
{
public:
	static constexpr void onError(const bzd::StringView&) { bzd::assert::isTrueConstexpr(false); }
};

class RuntimeAssert
{
public:
	static constexpr void onError(const bzd::StringView& view) { bzd::assert::isTrue(false, view.data()); }
};

class StreamFormatter
{
public:
	template <class T>
	using FormatterType = decltype(toStream(bzd::typeTraits::declval<bzd::OChannel&>(), bzd::typeTraits::declval<T>()));

	template <class T>
	using FormatterWithMetadataType = decltype(
		toStream(bzd::typeTraits::declval<bzd::OChannel&>(), bzd::typeTraits::declval<T>(), bzd::typeTraits::declval<const Metadata>()));
};

template <class T>
void printInteger(bzd::OChannel& stream, const T& value, const Metadata& metadata)
{
	switch (metadata.format)
	{
	case Metadata::Format::AUTO:
	case Metadata::Format::DECIMAL:
		bzd::format::toStream(stream, value);
		break;
	case Metadata::Format::BINARY:
		if (metadata.alternate)
		{
			stream.write(bzd::StringView{"0b"}.asBytes());
		}
		bzd::format::toStreamBin(stream, value);
		break;
	case Metadata::Format::HEXADECIMAL_LOWER:
		if (metadata.alternate)
		{
			stream.write(bzd::StringView{"0x"}.asBytes());
		}
		bzd::format::toStreamHex(stream, value);
		break;
	case Metadata::Format::HEXADECIMAL_UPPER:
		if (metadata.alternate)
		{
			stream.write(bzd::StringView{"0x"}.asBytes());
		}
		bzd::format::toStreamHex(stream, value, "0123456789ABCDEF");
		break;
	case Metadata::Format::OCTAL:
		if (metadata.alternate)
		{
			stream.write(bzd::StringView{"0o"}.asBytes());
		}
		bzd::format::toStreamOct(stream, value);
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
		bzd::format::toStream(stream, value);
		break;
	case Metadata::Format::FIXED_POINT:
		bzd::format::toStream(stream, value, (metadata.isPrecision) ? metadata.precision : 6);
		break;
	case Metadata::Format::FIXED_POINT_PERCENT:
		bzd::format::toStream(stream, value * 100., (metadata.isPrecision) ? metadata.precision : 6);
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

// Specialization of toStream for the native types

template <class T, bzd::typeTraits::EnableIf<bzd::typeTraits::isIntegral<T>, void>* = nullptr>
void toStream(bzd::OChannel& stream, const T& value, const Metadata& metadata)
{
	printInteger(stream, value, metadata);
}

template <class T, bzd::typeTraits::EnableIf<bzd::typeTraits::isFloatingPoint<T>, void>* = nullptr>
void toStream(bzd::OChannel& stream, const T& value, const Metadata& metadata)
{
	printFixedPoint(stream, value, metadata);
}

template <
	class T,
	bzd::typeTraits::EnableIf<bzd::typeTraits::isPointer<T> && !bzd::typeTraits::isConstructible<bzd::StringView, T>, void>* = nullptr>
void toStream(bzd::OChannel& stream, const T& value, const Metadata&)
{
	Metadata metadata{};
	metadata.format = Metadata::Format::HEXADECIMAL_LOWER;
	metadata.alternate = true;
	printInteger(stream, reinterpret_cast<SizeType>(value), metadata);
}

static void toStream(bzd::OChannel& stream, const bzd::StringView stringView, const Metadata& metadata)
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

/**
 * \brief Check the format context.
 *
 * Check the format context with the argument type, this to ensure type safety.
 * This function should only be used at compile time.
 */
template <SizeType N, class Adapter, class MetadataList, class T, bzd::typeTraits::EnableIf<(N > 0)>* = nullptr>
constexpr bool contextCheck(const MetadataList& metadataList, const T& tuple)
{
	auto value = tuple.template get<N - 1>();
	Adapter::assertTrue(HasFormatter<Adapter, decltype(value)>::value || HasFormatterWithMetadata<Adapter, decltype(value)>::value,
						"Argument type is not supported, it must contain a valid formatter.");

	bool usedAtLeastOnce = false;
	for (const auto& metadata : metadataList)
	{
		Adapter::assertTrue(metadata.index < T::size(), "The index specified is greater than the number of arguments provided");
		if (metadata.index == N - 1)
		{
			usedAtLeastOnce = true;
			switch (metadata.format)
			{
			case Metadata::Format::BINARY:
			case Metadata::Format::OCTAL:
			case Metadata::Format::HEXADECIMAL_LOWER:
			case Metadata::Format::HEXADECIMAL_UPPER:
				Adapter::assertTrue(bzd::typeTraits::isIntegral<decltype(value)>, "Argument must be an integral");
				break;
			case Metadata::Format::DECIMAL:
			case Metadata::Format::FIXED_POINT:
			case Metadata::Format::FIXED_POINT_PERCENT:
				Adapter::assertTrue(bzd::typeTraits::isArithmetic<decltype(value)>, "Argument must be arithmetic");
				break;
			case Metadata::Format::POINTER:
				break;
			case Metadata::Format::AUTO:
				break;
			}
		}
	}

	Adapter::assertTrue(usedAtLeastOnce, "At least one argument is not being used by the formating string");
	return contextCheck<N - 1, Adapter>(metadataList, tuple);
}

template <SizeType N, class Adapter, class MetadataList, class T, bzd::typeTraits::EnableIf<(N == 0)>* = nullptr>
constexpr bool contextCheck(const MetadataList& metadataList, const T&)
{
	return true;
}

template <class T>
constexpr bool contextValidate(const bzd::StringView& format, const T& tuple)
{
	using ConstexprAdapter = Adapter<ConstexprAssert, StreamFormatter>;
	bzd::VectorConstexpr<Metadata, 128> metadataList{};
	Parse<ConstexprAdapter> parser{format};

	for (const auto& result : parser)
	{
		if (result.metadata.hasValue())
		{
			metadataList.pushBack(result.metadata.value());
		}
	}

	return bzd::format::impl::contextCheck<T::size(), ConstexprAdapter>(metadataList, tuple);
}

template <SizeType... I, class... Args>
constexpr void toStreamRuntime(bzd::OChannel& stream, const bzd::StringView& format, bzd::meta::range::Type<I...>, Args&&... args)
{
	using bzd::format::impl::FormatterSpecialized;
	using RuntimeAdapter = Adapter<RuntimeAssert, StreamFormatter>;

	// Run-time call
	const bzd::Tuple<FormatterSpecialized<RuntimeAdapter, bzd::typeTraits::Decay<Args>>...> tuple{
		FormatterSpecialized<RuntimeAdapter, bzd::typeTraits::Decay<Args>>(args)...};
	const bzd::Vector<const bzd::format::impl::Formatter*, sizeof...(args)> argList{(&tuple.template get<I>())...};
	Parse<RuntimeAdapter> parser{format};

	for (const auto& result : parser)
	{
		if (!result.str.empty())
		{
			stream.write(result.str.asBytes());
		}
		if (result.metadata.hasValue())
		{
			const auto& metadata = result.metadata.value();
			argList[metadata.index]->print(stream, metadata);
		}
	}
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
constexpr void toStream(bzd::OChannel& out, const bzd::StringView& str, Args&&... args)
{
	// Run-time call
	impl::toStreamRuntime(out, str, bzd::meta::Range<0, sizeof...(Args)>{}, bzd::forward<Args>(args)...);
}

template <class ConstexprStringView, class... Args>
constexpr void toStream(bzd::OChannel& out, const ConstexprStringView& str, Args&&... args)
{
	// Compile-time format check
	constexpr const bzd::Tuple<bzd::typeTraits::Decay<Args>...> tuple{};
	constexpr const bool isValid = bzd::format::impl::contextValidate(ConstexprStringView::value(), tuple);
	// This line enforces compilation time evaluation
	static_assert(isValid, "Compile-time string format check failed.");

	// Run-time call
	bzd::format::toStream(out, ConstexprStringView::value(), bzd::forward<Args>(args)...);
}

} // namespace bzd::format
