#pragma once

#include "include/utility.h"
#include "include/container/string.h"
#include "include/container/string_view.h"
#include "include/system.h"
#include "include/format.h"

namespace bzd
{
	namespace log
	{
		enum class Level
		{
			CRITICAL = 0,
			ERROR = 1,
			WARNING = 2,
			INFO = 3,
			DEBUG = 4
		};

		namespace impl
		{
			class Buffer : public SingletonImpl<Buffer>
			{
			public:

			};
		}

		template <class... Args>
		void print(const bzd::StringView& fmt, Args&&... args) noexcept
		{
			bzd::String<128> message;
			bzd::format(message, fmt, bzd::forward<Args>(args)...);
			bzd::getOut().write(message);
		}
	}
}
