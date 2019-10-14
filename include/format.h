#pragma once

#include "include/container/string.h"
#include "include/container/iostream.h"
#include "include/container/string_stream.h"
#include "include/container/tuple.h"
#include "include/utility.h"
#include "include/to_string.h"
#include "include/assert_minimal.h"

#include <string.h>
#include <functional>
#include <iostream>

namespace bzd
{
	namespace impl
	{
		namespace format
		{
			template <class Stream>
			constexpr void formatConsumeStaticString(Stream& dest, bzd::StringView& format)
			{
				SizeType offset = 0;
				do
				{
					const auto index = format.find('{', offset);
					if (index == bzd::StringView::npos)
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

			constexpr SizeType getFormatArgIndexAndConsume(bzd::StringView& format, const SizeType current = 0)
			{
				bool isDefined = false;
				SizeType index = 0;
				while (format.size() && format.front() >= '0' && format.front() <= '9')
				{
					index = index * 10 + (format.front() - '0');
					isDefined = true;
					format.removePrefix(1);
				}

				bzd::assertTrue(format.size(), "Format ended abruptly");
				bzd::assertTrue(format.front() == ':' || format.front() == '}', "Missing ':' or '}'");
				if (format.front() == ':')
				{
					format.removePrefix(1);
				}

				return (isDefined) ? index : current;
			}

			template <class Args>
			constexpr void formatProcessString(bzd::OStream& dest, bzd::StringView& format, const Args& args)
			{
				SizeType currentIndex = 0;
				do
				{
					formatConsumeStaticString(dest, format);
					if (format.size())
					{
						const auto index = getFormatArgIndexAndConsume(format, currentIndex++);
						const auto arg = args.get(index);
						switch (format.front())
						{
						case 'i':
						case 'd':
							bzd::toString(dest, arg);
							format.removePrefix(1);
							break;
						case '}':
							bzd::toString(dest, arg);
							break;
						}
						if (format.front() == '}')
						{
							format.removePrefix(1);
						}
					}
				} while (format.size());
			}
		}
	}

	template <class... Args>
	constexpr void format(bzd::OStream& dest, bzd::StringView fmt, Args&&... args)
	{
		const bzd::Tuple<Args...> tuple(bzd::forward<Args>(args)...);
		bzd::impl::format::formatProcessString(dest, fmt, tuple);
	}

	template <class... Args>
	constexpr void format(bzd::interface::String& dest, bzd::StringView fmt, Args&&... args)
	{
		dest.clear();
		bzd::interface::StringStream sstream(dest);
		format(sstream, fmt, bzd::forward<Args>(args)...);
	}
}
