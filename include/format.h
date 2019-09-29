#pragma once

#include "include/container/string.h"
#include "include/container/iostream.h"
#include "include/container/string_stream.h"
#include "include/type_traits/fundamental.h"
#include "include/utility.h"

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

		template <class Handler, class Arg, class... Args>
		constexpr void processStringFormat(Handler&& callback, StringView& format, Arg&& arg, Args&&... args)
		{
			processStringFormat(callback, format);
			if (format.size())
			{
				switch (format.front())
				{
				case 'i':
				case 'd':
					{
						static_assert(typeTraits::isIntegral<Arg>::value, "Parameter is not an integral");
						bzd::String<10> buffer;
						toString(buffer, arg);
						callback(buffer.data());
					}
					break;
				}
				format.removePrefix(1);

				processStringFormat(callback, format, bzd::forward<Args>(args)...);
			}
		}

		template <class Handler>
		constexpr void processStringFormat(Handler&& callback, StringView& format)
		{
			SizeType offset = 0;
			do
			{
				const auto index = format.find('%', offset);
				if (index == StringView::npos)
				{
					callback(format);
					return;
				}
				
				callback(format.substr(0, index));
				format.removePrefix(index + 1);
 				offset = (format.front() == '%') ? 1 : 0;

			} while (offset);
		}

		template <class... Args>
		void toString(bzd::OStream& dest, StringView format, Args&&... args)
		{
			processStringFormat([&](StringView str) {
				dest.write(str);
			}, format, bzd::forward<Args>(args)...);
		}

		template <class... Args>
		void toString(bzd::interface::String& dest, StringView format, Args&&... args)
		{
			dest.clear();
			bzd::interface::StringStream sstream(dest);
			toString(sstream, format, bzd::forward<Args>(args)...);
		}
	}
}
