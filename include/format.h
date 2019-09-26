#pragma once

#include "include/container/string.h"
#include "include/type_traits/fundamental.h"

#include <functional>

namespace bzd
{
	namespace format
	{
		namespace impl
		{
			template <class T, SizeType Base = 10>
			void integerToString(interface::String& str, const T n)
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
		typename bzd::typeTraits::enableIf<typeTraits::isIntegral<T>::value, Q>::type toString(interface::String& str, const T data)
		{
			impl::integerToString(str, data);
		}

		template <typename Iterator, typename Handler>
		constexpr void parse_format_string(Iterator it, Handler&& handler) {

			auto start = it;
			while (*it) {
				char ch = *it;
				++it;
				if (ch != '{' && ch != '}') continue;
				if (*it == ch) {
				handler.on_text(start, it);
				start = ++it;
				continue;
				}
				if (ch == '}') {
				handler.on_error("unmatched '}' in format string");
				return;
				}
				handler.on_text(start, it - 1);
				// parse format specs
				start = ++it;
			}
			handler.on_text(start, it);
		}

		void format(StringView fmt)
		{
			struct Temp
			{
				std::function<void(StringView::ConstIterator, StringView::ConstIterator)> on_text;
				std::function<void(const char*)> on_error;
			};

			const Temp temp{
				[](StringView::ConstIterator start, StringView::ConstIterator it) {
					std::cout << *start << "," << *it << std::endl;
				},
				[](StringView error) {
					std::cout << error.data() << std::endl;
				}
			};

			parse_format_string(fmt.begin(), temp);
		}
	}
}
