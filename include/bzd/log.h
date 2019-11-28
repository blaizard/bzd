#pragma once

#include "bzd/utility/forward.h"
#include "bzd/container/string.h"
#include "bzd/container/string_view.h"
#include "bzd/system.h"
#include "bzd/format/format.h"

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

		template <class... Args>
		void print(const bzd::StringView& fmt, Args&&... args) noexcept
		{
			bzd::String<128> message;
			bzd::format::toString(message, fmt, bzd::forward<Args>(args)...);
			bzd::getOut().write(message);
		}
	}
}
