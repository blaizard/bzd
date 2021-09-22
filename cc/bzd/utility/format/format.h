#pragma once

#include "cc/bzd/container/array.h"
#include "cc/bzd/container/optional.h"
#include "cc/bzd/container/string.h"
#include "cc/bzd/container/string_view.h"
#include "cc/bzd/container/tuple.h"
#include "cc/bzd/container/vector.h"
#include "cc/bzd/core/assert/minimal.h"
#include "cc/bzd/core/async.h"
#include "cc/bzd/core/channel.h"
#include "cc/bzd/meta/range.h"
#include "cc/bzd/type_traits/decay.h"
#include "cc/bzd/type_traits/declval.h"
#include "cc/bzd/type_traits/is_arithmetic.h"
#include "cc/bzd/type_traits/is_constructible.h"
#include "cc/bzd/type_traits/is_floating_point.h"
#include "cc/bzd/type_traits/is_integral.h"
#include "cc/bzd/type_traits/is_pointer.h"
#include "cc/bzd/utility/format/integral.h"

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
class Parser
{
public:
	constexpr Parser(bzd::StringView format) noexcept : iteratorBegin_{format} {}

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
	using FormatterWithMetadataType = decltype(toStream(
		bzd::typeTraits::declval<bzd::OChannel&>(), bzd::typeTraits::declval<T>(), bzd::typeTraits::declval<const Metadata>()));

	using FormatterTransportType = bzd::OChannel;
	using FormatterReturnType = Async<void>;

public:
	template <class T, bzd::typeTraits::EnableIf<!HasFormatterWithMetadata<StreamFormatter, T>::value, void>* = nullptr>
	static Async<void> process(bzd::OChannel& stream, const T& value, const Metadata&) noexcept
	{
		co_await toStream(stream, value);
	}

	template <class T, bzd::typeTraits::EnableIf<HasFormatterWithMetadata<StreamFormatter, T>::value, void>* = nullptr>
	static Async<void> process(bzd::OChannel& stream, const T& value, const Metadata& metadata) noexcept
	{
		co_await toStream(stream, value, metadata);
	}
};

class StringFormatter
{
public:
	template <class T>
	using FormatterType = decltype(toString(bzd::typeTraits::declval<bzd::interface::String&>(), bzd::typeTraits::declval<T>()));

	template <class T>
	using FormatterWithMetadataType = decltype(toString(
		bzd::typeTraits::declval<bzd::interface::String&>(), bzd::typeTraits::declval<T>(), bzd::typeTraits::declval<const Metadata>()));

	using FormatterTransportType = bzd::interface::String;
	using FormatterReturnType = void;

public:
	template <class T, bzd::typeTraits::EnableIf<!HasFormatterWithMetadata<StringFormatter, T>::value, void>* = nullptr>
	static constexpr void process(bzd::interface::String& str, const T& value, const Metadata&) noexcept
	{
		toString(str, value);
	}

	template <class T, bzd::typeTraits::EnableIf<HasFormatterWithMetadata<StringFormatter, T>::value, void>* = nullptr>
	static constexpr void process(bzd::interface::String& str, const T& value, const Metadata& metadata) noexcept
	{
		toString(str, value, metadata);
	}
};

constexpr bzd::StringView processCommon(const bzd::StringView stringView, const Metadata& metadata)
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

template <class T, bzd::typeTraits::EnableIf<bzd::typeTraits::isIntegral<T>, void>* = nullptr>
constexpr void toString(bzd::interface::String& str, const T& value, const Metadata& metadata)
{
	switch (metadata.format)
	{
	case Metadata::Format::AUTO:
	case Metadata::Format::DECIMAL:
		bzd::format::toString(str, value);
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
		bzd::format::toStringHex(str, value, "0123456789ABCDEF");
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

template <class T, bzd::typeTraits::EnableIf<bzd::typeTraits::isFloatingPoint<T>, void>* = nullptr>
constexpr void toString(bzd::interface::String& str, const T& value, const Metadata& metadata)
{
	switch (metadata.format)
	{
	case Metadata::Format::AUTO:
	case Metadata::Format::DECIMAL:
		bzd::format::toString(str, value);
		break;
	case Metadata::Format::FIXED_POINT:
		bzd::format::toString(str, value, (metadata.isPrecision) ? metadata.precision : 6);
		break;
	case Metadata::Format::FIXED_POINT_PERCENT:
		bzd::format::toString(str, value * 100., (metadata.isPrecision) ? metadata.precision : 6);
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

template <
	class T,
	bzd::typeTraits::EnableIf<bzd::typeTraits::isPointer<T> && !bzd::typeTraits::isConstructible<bzd::StringView, T>, void>* = nullptr>
constexpr void toString(bzd::interface::String& str, const T& value, const Metadata&)
{
	Metadata metadata{};
	metadata.format = Metadata::Format::HEXADECIMAL_LOWER;
	metadata.alternate = true;
	toString(str, reinterpret_cast<SizeType>(value), metadata);
}

constexpr void toString(bzd::interface::String& str, const bzd::StringView stringView, const Metadata& metadata)
{
	str += processCommon(stringView, metadata);
}

// Specialization of toStream for the native types

template <class T,
		  bzd::typeTraits::EnableIf<bzd::typeTraits::isIntegral<T> || bzd::typeTraits::isFloatingPoint<T> ||
										(bzd::typeTraits::isPointer<T> && !bzd::typeTraits::isConstructible<bzd::StringView, T>),
									void>* = nullptr>
Async<void> toStream(bzd::OChannel& stream, const T& value, const Metadata& metadata)
{
	bzd::String<80> str{};
	toString(str, value, metadata);
	co_await stream.write(str.asBytes());
}

inline Async<void> toStream(bzd::OChannel& stream, const bzd::StringView stringView, const Metadata& metadata)
{
	co_await stream.write(processCommon(stringView, metadata).asBytes());
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
constexpr bool contextCheck(const MetadataList&, const T&)
{
	return true;
}

template <class T>
constexpr bool contextValidate(const bzd::StringView& format, const T& tuple)
{
	using ConstexprAdapter = Adapter<ConstexprAssert, StreamFormatter>;
	bzd::VectorConstexpr<Metadata, 128> metadataList{};
	Parser<ConstexprAdapter> parser{format};

	for (const auto& result : parser)
	{
		if (result.metadata.hasValue())
		{
			metadataList.pushBack(result.metadata.value());
		}
	}

	return bzd::format::impl::contextCheck<T::size(), ConstexprAdapter>(metadataList, tuple);
}

template <class Adapter>
class Formatter
{
public:
	using TransportType = typename Adapter::FormatterTransportType;
	using ReturnType = typename Adapter::FormatterReturnType;

public:
	template <class... Args>
	static constexpr auto make(Args&&... args) noexcept
	{
		return makeInternal(bzd::meta::Range<0, sizeof...(Args)>{}, bzd::forward<Args>(args)...);
	}

private:
	template <class Lambdas, class LambdasErased, SizeType... I>
	class FormatterType
	{
	public:
		constexpr FormatterType(Lambdas& lambdas, const LambdasErased& typeErasedLambdas) noexcept :
			lambdas_{lambdas}, typeErasedLambdas_{typeErasedLambdas}, fcts_{(typeErasedLambdas_.template get<I>())...},
			ptrs_{(reinterpret_cast<const void*>(&lambdas_.template get<I>()))...}
		{
		}

		constexpr ReturnType process(TransportType& transport, const Metadata& metadata) const noexcept
		{
			const auto index = metadata.index;
			fcts_[index](ptrs_[index], transport, metadata);
		}

		ReturnType processAsync(TransportType& transport, const Metadata& metadata) const noexcept
		{
			const auto index = metadata.index;
			co_await fcts_[index](ptrs_[index], transport, metadata);
		}

	private:
		const Lambdas lambdas_;
		const LambdasErased typeErasedLambdas_;
		const bzd::Array<ReturnType (*)(const void*, TransportType&, const Metadata&), Lambdas::size()> fcts_;
		const bzd::Array<const void*, Lambdas::size()> ptrs_;
	};

private:
	template <SizeType... I, class... Args>
	static constexpr auto makeInternal(bzd::meta::range::Type<I...>, Args&&... args) noexcept
	{
		// Make the actual lambda
		const auto lambdas = bzd::makeTuple([&args](TransportType & transport, const Metadata& metadata) -> auto {
			return Adapter::process(transport, args, metadata);
		}...);
		using LambdaTupleType = decltype(lambdas);
		// Make the lambda with type erasure
		const auto typeErasedLambdas = bzd::makeTuple([](const void* lambda, TransportType& out, const Metadata& metadata) -> auto {
			return (*reinterpret_cast<const typename LambdaTupleType::template ItemType<I>*>(lambda))(out, metadata);
		}...);
		using LambdaTypeErasedTupleType = decltype(typeErasedLambdas);

		return FormatterType<LambdaTupleType, LambdaTypeErasedTupleType, I...>{lambdas, typeErasedLambdas};
	}
};

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
template <class ConstexprStringView, class... Args>
Async<void> toStream(bzd::OChannel& stream, const ConstexprStringView& format, Args&&... args)
{
	if constexpr (!bzd::typeTraits::isConstructible<bzd::StringView, ConstexprStringView>)
	{
		// Compile-time format check
		constexpr const bzd::Tuple<bzd::typeTraits::Decay<Args>...> tuple{};
		constexpr const bool isValid = bzd::format::impl::contextValidate(ConstexprStringView::value(), tuple);
		// This line enforces compilation time evaluation
		static_assert(isValid, "Compile-time string format check failed.");

		// Run-time call
		co_await bzd::format::toStream(stream, ConstexprStringView::value(), bzd::forward<Args>(args)...);
	}
	else
	{
		using RuntimeAdapter = impl::Adapter<impl::RuntimeAssert, impl::StreamFormatter>;

		const auto formatter = impl::Formatter<RuntimeAdapter>::make(bzd::forward<Args>(args)...);
		impl::Parser<RuntimeAdapter> parser{format};

		// Run-time call
		for (const auto& result : parser)
		{
			if (!result.str.empty())
			{
				co_await stream.write(result.str.asBytes());
			}
			if (result.metadata.hasValue())
			{
				co_await formatter.processAsync(stream, result.metadata.value());
			}
		}
	}
}

template <class ConstexprStringView, class... Args>
constexpr void toString(bzd::interface::String& str, const ConstexprStringView& format, Args&&... args)
{
	if constexpr (!bzd::typeTraits::isConstructible<bzd::StringView, ConstexprStringView>)
	{
		// Compile-time format check
		constexpr const bzd::Tuple<bzd::typeTraits::Decay<Args>...> tuple{};
		constexpr const bool isValid = bzd::format::impl::contextValidate(ConstexprStringView::value(), tuple);
		// This line enforces compilation time evaluation
		static_assert(isValid, "Compile-time string format check failed.");

		// Run-time call
		bzd::format::toString(str, ConstexprStringView::value(), bzd::forward<Args>(args)...);
		return;
	}
	else
	{
		using RuntimeAdapter = impl::Adapter<impl::RuntimeAssert, impl::StringFormatter>;

		const auto formatter = impl::Formatter<RuntimeAdapter>::make(bzd::forward<Args>(args)...);
		impl::Parser<RuntimeAdapter> parser{format};

		// Run-time call
		for (const auto& result : parser)
		{
			if (!result.str.empty())
			{
				str.append(result.str);
			}
			if (result.metadata.hasValue())
			{
				formatter.process(str, result.metadata.value());
			}
		}
	}
}

} // namespace bzd::format
