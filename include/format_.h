#pragma once

#include "include/type_traits/fundamental.h"
#include "include/container/constexpr_string_view.h"
#include "include/container/string_view.h"
#include "include/container/string.h"
#include "include/container/tuple.h"
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
			 * Simple vector container
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

			struct RetIndex
			{
				bzd::SizeType index;
				bzd::SizeType offset;
			};

			template <class Ctx>
			constexpr RetIndex parseIndex(Ctx& context, const bzd::StringView& format, bzd::SizeType offset, const bzd::SizeType current)
			{
				bool isDefined = false;
				bzd::SizeType index = 0;
				for (; offset < format.size() && format[offset] >= '0' && format[offset] <= '9'; ++offset)
				{
					index = index * 10 + (format[offset] - '0');
					isDefined = true;
				}

				if (offset >= format.size())
					throw "Format ended abruptly";

				if (format[offset] == ':')
				{
					++offset;
					if (offset >= format.size())
						throw "Format ended abruptly";
				}
				else if (format[offset] != '}')
					throw "Missing ':' or '}'";

				return RetIndex{(isDefined) ? index : current, offset};
			}

			enum class FormatType
			{
				AUTO, 
				INTEGER,
				STRING
			};

			template <class T>
			struct formatter
			{
				void format(bzd::OStream& out, T temp)
				{
				}
			};

			template <>
			struct formatter<int>
			{
				void format(bzd::OStream& out, int temp)
				{
				}
			};

			struct Metadata
			{
				bzd::SizeType index;
				FormatType format;
				bzd::SizeType offset;
			};

			template <class Ctx>
			constexpr Metadata parseMetadata(Ctx& context, const bzd::StringView& format, bzd::SizeType offset, const bzd::SizeType current)
			{
				const auto ret = parseIndex(context, format, offset, current);
				offset = ret.offset;
				FormatType f = FormatType::AUTO;
				if (format[offset] != '}')
				{
					switch (format[offset])
					{
					case 'd':
					case 'i':
						f = FormatType::INTEGER;
						break;
					case 's':
						f = FormatType::STRING;
						break;
					default:
						throw "Unknown format";
					}
					++offset;
					if (offset >= format.size())
						throw "Format ended abruptly";
				}
				if (format[offset] != '}')
					throw "Missing '}'";
				++offset;

				return Metadata{ret.index, f, offset};
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

					if (c != '{')
					{
						context.onError("'}' must be part of a pair '{...}' or doubled '}}'");
					}

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
				// Make a copy of the format string
				if (parseStaticString(context, format))
				{
					if (format.front() != '{')
					{
						context.onError("Unexpected return state for parseStaticString");
					}
				}
					bzd::SizeType index = 0;
					bzd::SizeType offset = 0;

				const auto newOffset = format.find('{', offset);
				context.addSubstring(format.substr(offset, newOffset - offset));

				offset = newOffset;

				if (offset == bzd::StringView::npos)
				{
					// End
					return;
				}

				// Point after the '{'
				++offset;

				// Parse the format
				const auto ret = parseMetadata(context, format, offset, index);
				if (ret.index >= tuple.size())
				{
					context.onError("The index specified is greater than the number of arguments.");
				}
				switch (ret.format)
				{
				case FormatType::AUTO:
					break;
				case FormatType::INTEGER:
				//	if (!bzd::typeTraits::isIntegral<decltype(value)>::value)
				//		throw "This is not a integral";
					break;
				case FormatType::STRING:
					throw "This is not a string";
				default:
					throw "Unsupported type";
				}

				context.addMetadata(ret);
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
						context.assertTrue(bzd::typeTraits::isIntegral<decltype(value)>::value, "Not an integral");
					}
				}

				context.assertTrue(usedAtLeastOnce, "At least one argument is not being used by the formating string");

				contextCheck<N-1>(context, tuple);
				return true;
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
		constexpr const bzd::Tuple<Args...> tuple;
		constexpr const auto context = bzd::impl::format::contextBuild(F::data(), tuple);
		static_assert(bzd::impl::format::contextCheck<tuple.size()>(context, tuple), "String format check failed");

		bzd::impl::format::print(out, f.str(), tuple);
	}
}
