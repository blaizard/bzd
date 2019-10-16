#pragma once

#include "include/type_traits/fundamental.h"
#include "include/container/constexpr_string_view.h"
#include "include/container/string_view.h"
#include "include/container/string.h"
#include "include/container/tuple.h"
#include "include/container/variant.h"
#include "include/container/vector.h"
#include "include/container/iostream.h"
#include "include/to_string.h"
#include "include/system.h"

#include <array>
#include <iostream>
#include <cstdarg>

namespace bzd
{
	namespace impl
	{
		namespace format
		{
			/**
			 * Simple vector container working with conxtexpr
			 */
			template <class T, SizeType N>
			class ConstexprVector
			{
				using SelfType = ConstexprVector<T, N>;
				using DataType = T;
			public:
				class Iterator
				{
				public:
					constexpr Iterator(const SelfType& container, const SizeType index) : container_(&container), index_(index) {}
					constexpr Iterator& operator++() noexcept { ++index_; return *this; }
					constexpr bool operator==(const Iterator& it) const noexcept { return it.index_ == index_; }
					constexpr bool operator!=(const Iterator& it) const noexcept { return !(it == *this); }
					constexpr const DataType& operator*() const { return (*container_)[index_]; }

				private:
					const SelfType* container_;
					SizeType index_;
				};

			public:
				constexpr ConstexprVector() noexcept {}
				constexpr Iterator begin() const noexcept { return Iterator(*this, 0); }
				constexpr Iterator end() const noexcept { return Iterator(*this, size()); }
				constexpr SizeType size() const noexcept { return size_; }
				constexpr SizeType capacity() const noexcept { return N; }
				constexpr void push_back(const T& element) noexcept { data_[size_++] = element; }
				constexpr DataType& operator[](const SizeType index) { return data_[index]; }
				constexpr const DataType& operator[](const SizeType index) const { return data_[index]; }

			private:
				T data_[N] = {};
				SizeType size_ = 0;
			};

			struct Metadata
			{
				enum class Align
				{
					AUTO,
					LEFT,
					RIGHT,
					CENTER,
					SIGN_PADDING
				};

				enum class Sign
				{
					AUTO,
					ALWAYS,
					ONLY_NEGATIVE,
					LEADING_SPACE
				};

				enum class Format
				{
					AUTO,
					BINARY_LOWER,
					BINARY_UPPER,
					DECIMAL,
					OCTAL,
					HEXADECIMAL_LOWER,
					HEXADECIMAL_UPPER,
					FIXED_POINT,
					FIXED_POINT_PERCENT,
					POINTER
				};

				bzd::SizeType index = 0;
				Align align = Align::AUTO;
				char alignChar = ' ';
				Sign sign = Sign::AUTO;
				bool alternate = false;
				bool isWidth = false;
				bzd::SizeType width = 0;
				bool isPrecision = false;
				bzd::SizeType precision = 0;
				Format format = Format::AUTO;
			};

			class Arg
			{
			public:
				constexpr Arg(const int value = 0) : intValue(value) {}
				constexpr Arg(const unsigned int value) : uIntValue(value) {}
				constexpr Arg(const long long int value) : longLongValue(value) {}
				constexpr Arg(const unsigned long long int value) : uLongLongValue(value) {}
				constexpr Arg(const bool value) : boolValue(value) {}
				constexpr Arg(const char value) : charValue(value) {}
				constexpr Arg(const float value) : floatValue(value) {}
				constexpr Arg(const double value) : doubleValue(value) {}
				constexpr Arg(const long double value) : longDoubleValue(value) {}
				constexpr Arg(const void* value) : pointer(value) {}
				constexpr Arg(const char* value) : string(value) {}
				constexpr Arg(const bzd::StringView value) : stringView({value.data(), value.size()}) {}

			public:
				union
				{
					int intValue;
					unsigned int uIntValue;
					long long int longLongValue;
					unsigned long long int uLongLongValue;
					bool boolValue;
					char charValue;
					float floatValue;
					double doubleValue;
					long double longDoubleValue;
					const void* pointer;
					const char* string;
					struct { const char* data; SizeType size; } stringView;
				};
			};

			using ArgList = bzd::interface::Vector<Arg>;
/*
  none_type,

  cstring_type,
  string_type,
  pointer_type,
  custom_type

  ------

    int int_value;
    unsigned uint_value;
    long long long_long_value;
    unsigned long long ulong_long_value;
    int128_t int128_value;
    uint128_t uint128_value;
    bool bool_value;
    char_type char_value;
    float float_value;
    double double_value;
    long double long_double_value;
    const void* pointer;
    string_value<char_type> string;
    custom_value<Context> custom;
    const named_arg_base<char_type>* named_arg;
*/


			/**
			 * Parse an unsigned integer
			 */
			constexpr bool parseUnsignedInteger(bzd::StringView& format, bzd::SizeType& integer)
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

			// Align

			constexpr bool parseIsAlignSetter(const char c, Metadata& metadata)
			{
				switch (c)
				{
				case '<':
					metadata.align = Metadata::Align::LEFT;
					return true;
				case '>':
					metadata.align = Metadata::Align::RIGHT;
					return true;
				case '^':
					metadata.align = Metadata::Align::CENTER;
					return true;
				case '=':
					metadata.align = Metadata::Align::SIGN_PADDING;
					return true;
				}
				return false;
			}

			template <class Ctx>
			constexpr void parseAlign(Ctx& context, bzd::StringView& format, Metadata& metadata)
			{
				if (format.size() >= 2 && parseIsAlignSetter(format[1], metadata))
				{
					metadata.alignChar = format.front();
					format.removePrefix(2);
				}
				else if (parseIsAlignSetter(format.front(), metadata))
				{
					format.removePrefix(1);
				}
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
				case ' ':
					metadata.sign = Metadata::Sign::LEADING_SPACE;
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
			 * Format compatible with python format:
			 * 
			 * format_spec ::=  [[fill]align][sign][#][0][width][.precision][type]
			 * fill        ::=  <any character>
			 * align       ::=  "<" | ">" | "=" | "^"
			 * sign        ::=  "+" | "-" | " "
			 * width       ::=  integer
			 * precision   ::=  integer
			 * type        ::=  "b" | "B" | "d" | "f" | "o" | "x" | "X" | "f" | "p" | "%"
			 */
			template <class Ctx>
			constexpr Metadata parseMetadata(Ctx& context, bzd::StringView& format, const bzd::SizeType current)
			{
				const auto endIndex = format.find('}');
				context.assertTrue(endIndex != bzd::StringView::npos, "Missing closing '}' for the replacement field");
				auto metadataStr = format.substr(0, endIndex + 1);

				Metadata metadata;

				// Look for the index
				metadata.index = parseIndex(context, metadataStr, current);
				context.assertTrue(metadataStr.size() > 0, "Replacement field format ended abruptly (after parseIndex)");

				// Parse align: [[fill]align]
				parseAlign(context, metadataStr, metadata);
				context.assertTrue(metadataStr.size() > 0, "Replacement field format ended abruptly (after parseAlign)");

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

				// Parse sign-aware zero padding [0]
				if (metadataStr.front() == '0')
				{
					context.assertTrue(metadata.align == Metadata::Align::AUTO, "Mismatch between align format type");
					metadata.alignChar = '0';
					metadata.align = Metadata::Align::SIGN_PADDING;
					metadataStr.removePrefix(1);
					context.assertTrue(metadataStr.size() > 0, "Replacement field format ended abruptly (after '0')");
				}

				// Parse width [width]
				metadata.isWidth = parseUnsignedInteger(metadataStr, metadata.width);
				context.assertTrue(metadataStr.size() > 0, "Replacement field format ended abruptly (after width)");

				// Parse precision [.precision]
				if (metadataStr.front() == '.')
				{
					metadata.isPrecision = parseUnsignedInteger(metadataStr, metadata.precision);
					context.assertTrue(metadataStr.size() > 0, "Replacement field format ended abruptly (after precision)");
				}

				// Parse type [type]
				if (metadataStr.front() != '}')
				{
					switch (metadataStr.front())
					{
					case 'b':
						metadata.format = Metadata::Format::BINARY_LOWER;
						break;
					case 'B':
						metadata.format = Metadata::Format::BINARY_UPPER;
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
						context.onError("Unsupported conversion format");
					}
					metadataStr.removePrefix(1);
					context.assertTrue(metadataStr.size() > 0, "Replacement field format ended abruptly (after type)");
				}
				context.assertTrue(metadataStr.front() == '}', "Invalid format for replacement field, expecting '}'");

				format.removePrefix(endIndex + 1);
				return metadata;
			}

			/**
			 * Parse a static string and call addSubstring to the context information
			 * when needed.
			 * 
			 * This function returns either when the string is consumed (return false)
			 * or when a metadata has been identified.
			 */
			template <class Ctx>
			constexpr bool parseStaticString(Ctx& context, bzd::StringView& format)
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
						context.addSubstring(format.substr(0, offset));
						format.removePrefix(offset + 1);
						offset = 0;
						continue;
					}

					context.assertTrue(c == '{', "'}' must be part of a pair '{...}' or doubled '}}'");

					--offset;
					break;
				}

				if (offset)
				{
					context.addSubstring(format.substr(0, offset));
					format.removePrefix(offset);
				}

				return format.size();
			}

			template <class Ctx, class T>
			constexpr void parse(Ctx& context, bzd::StringView format, const T& tuple)
			{
				bzd::SizeType autoIndex = 0;
				do
				{
					if (parseStaticString(context, format))
					{
						context.assertTrue(format.front() == '{', "Unexpected return state for parseStaticString");
						context.assertTrue(format.size() > 1, "Unexpected return state for parseStaticString");
						format.removePrefix(1);
						const auto metadata = parseMetadata(context, format, autoIndex++);
						context.assertTrue(metadata.index < tuple.size(), "The index specified is greater than the number of arguments provided");
						context.addMetadata(metadata);
					}
				} while (format.size() > 0);
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

			class CheckContext : public ConstexprVector<Metadata, 128>
			{
			public:
				constexpr CheckContext() = default;
				constexpr void addSubstring(const bzd::StringView&) {}
				constexpr void addMetadata(const Metadata& metadata) { push_back(metadata); }
				void onError(const bzd::StringView& message) const;
			};

			class PrintContext
			{
			public:
				constexpr PrintContext(bzd::OStream& stream) : stream_(stream) {}
				void addSubstring(const bzd::StringView& str) { stream_.write(str); }
				void addMetadata(const Metadata& metadata) {}
				void onError(const bzd::StringView& message) const {}

			private:
				bzd::OStream& stream_;
			};

			template <class T>
			constexpr Context<CheckContext> contextBuild(const bzd::StringView& format, const T& tuple)
			{
				Context<CheckContext> ctx;
				parse(ctx, format, tuple);
				return ctx;
			}

			template <class T>
			void print(bzd::OStream& stream, const bzd::StringView& format, const T& tuple)
			{
				Context<PrintContext> ctx(stream);
				parse(ctx, format, tuple);
			}

			/**
			 * \brief Check the format context.
			 * 
			 * Check the format context with the argument type, this to ensure type safety.
			 * This function should only be used at compile time.
			 */
			template <SizeType N, class Ctx, class T, typename bzd::typeTraits::enableIf<(N > 0), void>::type* = nullptr>
			constexpr bool contextCheck(const Ctx& context, const T& tuple)
			{
				auto value = tuple.template get<N-1>();
				bool usedAtLeastOnce = false;
				for (const auto& metadata : context)
				{
					if (metadata.index == N - 1)
					{
						usedAtLeastOnce = true;
						switch (metadata.format)
						{
						case Metadata::Format::BINARY_LOWER:
						case Metadata::Format::BINARY_UPPER:
						case Metadata::Format::OCTAL:
						case Metadata::Format::HEXADECIMAL_LOWER:
						case Metadata::Format::HEXADECIMAL_UPPER:
							context.assertTrue(bzd::typeTraits::isIntegral<decltype(value)>::value, "Argument must an integral");
							break;
						case Metadata::Format::DECIMAL:
						case Metadata::Format::FIXED_POINT:
						case Metadata::Format::FIXED_POINT_PERCENT:
							context.assertTrue(bzd::typeTraits::isArithmetic<decltype(value)>::value, "Argument must arithmetic");
							break;
						case Metadata::Format::POINTER:
							break;
						case Metadata::Format::AUTO:
							break;
						}
					}
				}

				context.assertTrue(usedAtLeastOnce, "At least one argument is not being used by the formating string");
				return contextCheck<N-1>(context, tuple);
			}

			template <SizeType N, class Ctx, class T, typename bzd::typeTraits::enableIf<(N == 0), void>::type* = nullptr>
			constexpr bool contextCheck(const Ctx&, const T&)
			{
				return true;
			}
		}
	}

	template <class F, class... Args>
	constexpr inline void format(bzd::OStream& out, const F& f, Args&&... args)
	{
		// Compile-time format check
		constexpr const bzd::Tuple<typename bzd::decay<Args>::type...> tuple;
		constexpr const auto context = bzd::impl::format::contextBuild(F::data(), tuple);
		static_assert(bzd::impl::format::contextCheck<tuple.size()>(context, tuple), "String format check failed");

		bzd::Vector<bzd::impl::format::Arg, tuple.size()> argList;
		for (SizeType i = 0; i < tuple.size(); ++i)
		{

		}

		// Run-time call
		bzd::impl::format::print(out, f.str(), tuple);
	}
}
