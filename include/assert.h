#pragma once

#include "include/container/string.h"
#include "include/container/string_view.h"
#include "include/format.h"
#include "include/system.h"

namespace bzd
{
	template <class... Args>
	constexpr void assertTrue(const bool condition, const bzd::StringView& fmt = "", Args&&... args)
	{
		if (!condition)
		{
			bzd::String<128> message;
			bzd::format(message, fmt, bzd::forward<Args>(args)...);
			bzd::panic();
		}
	}
}
