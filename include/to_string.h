#pragma once

#include "include/container/string.h"
#include "include/container/iostream.h"
#include "include/container/string_stream.h"
#include "include/type_traits/fundamental.h"
#include "include/utility.h"

namespace bzd
{
	namespace impl
	{
		namespace to_string
		{
			template <class T, SizeType Base = 10>
			constexpr void integer(interface::String& str, const T n)
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
	}

	template <class T, typename bzd::typeTraits::enableIf<typeTraits::isIntegral<T>::value, T>::type* = nullptr>
	constexpr void toString(bzd::OStream& stream, const T data)
	{
		bzd::String<40> buffer; // 40 is a the length of the max value of an 128-bit in base 10
		bzd::impl::to_string::integer(buffer, data);
		stream.write(buffer);
	}

	void toString(bzd::OStream& stream, const bzd::StringView& data)
	{
		stream.write(data);
	}

	template <class T>
	constexpr void toString(bzd::interface::String& str, T&& data)
	{
		str.clear();
		bzd::interface::StringStream sstream(str);
		toString(static_cast<bzd::OStream&>(sstream), bzd::forward<T>(data));
	}
}
