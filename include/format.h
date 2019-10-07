#pragma once

#include "include/container/string.h"
#include "include/container/iostream.h"
#include "include/container/string_stream.h"
#include "include/container/tuple.h"
#include "include/type_traits/fundamental.h"
#include "include/utility.h"

#include <string.h>
#include <functional>

namespace bzd
{
	namespace format
	{
		namespace impl
		{
			template <class T, SizeType Base = 10>
			constexpr void integerToString(interface::String& str, const T n)
			{
				constexpr const char* const digits = "0123456789abcdef";
				T number = (n < 0) ? -n : n;
				auto index = str.capacity();

				if (index)
				{
					do
					{
						const int digit = digits[static_cast<int>(number % Base)];
						number /= Base;
						str[--index] = static_cast<char>(digit);
					} while (number && index);

					if (n < 0 && index)
					{
						str[--index] = '-';
					}

					memmove(str.data(), &str[index], str.capacity() - index);
				}
				str.resize(str.capacity() - index);
			}
		}

		template <class T, class Q = void>
		constexpr typename bzd::typeTraits::enableIf<typeTraits::isIntegral<T>::value, Q>::type toString(interface::String& str, const T data)
		{
			impl::integerToString(str, data);
		}

		template <class Stream>
		constexpr void formatConsumeStaticString(Stream& dest, StringView& format)
		{
			SizeType offset = 0;
			do
			{
				const auto index = format.find('{', offset);
				if (index == StringView::npos)
				{
					dest.write(format);
					format.clear();
					return;
				}

				dest.write(format.substr(0, index));
				format.removePrefix(index + 1);
 				offset = (format.front() == '{') ? 1 : 0;

			} while (offset);
		}

		constexpr SizeType getFormatArgIndexAndConsume(StringView& format, const SizeType current = 0)
		{
			bool isDefined = false;
			SizeType index = 0;
			while (format.size() && format.front() >= '0' && format.front() <= '9')
			{
				index = index * 10 + (format.front() - '0');
				isDefined = true;
				format.removePrefix(1);
			}

			if (format.size() && format.front() == ':')
			{
				format.removePrefix(1);
			}

			return (isDefined) ? index : current;
		}

		template <class Stream, class Args>
		constexpr void formatProcessString(Stream& dest, StringView& format, const Args& args)
		{
			do
			{
				formatConsumeStaticString(dest, format);
				if (format.size())
				{
					switch (format.front())
					{
					case 'i':
					case 'd':
						{
							const auto index = getFormatArgIndexAndConsume(format);
							const auto arg = args.template get<int>(index);
							bzd::String<10> buffer;
							toString(buffer, arg);
							dest.write(static_cast<bzd::StringView>(buffer.data()));
						}
						break;
					}
					format.removePrefix(1);
					if (format.front() == '}')
					{
						format.removePrefix(1);
					}
				}
			} while (format.size());
		}

		template <class... Args>
		constexpr void toString(bzd::OStream& dest, StringView format, Args&&... args)
		{
			//constexpr const bzd::Tuple<Args...> tuple(bzd::forward<Args>(args)...);
			constexpr const bzd::Tuple<int> tuple(42);
			formatProcessString(dest, format, tuple);
		}

		template <class... Args>
		constexpr void toString(bzd::interface::String& dest, StringView format, Args&&... args)
		{
			dest.clear();
			bzd::interface::StringStream sstream(dest);
			toString(sstream, format, bzd::forward<Args>(args)...);
		}
	}
}
