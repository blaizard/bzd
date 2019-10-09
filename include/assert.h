#pragma once

#include "include/container/string_view.h"
#include "include/log.h"
#include "include/system.h"

namespace bzd
{
	template <class... Args>
	constexpr void assertTrue(const bool condition, const bzd::StringView& fmt = "", Args&&... args)
	{
		if (!condition)
		{
			bzd::log::print(fmt, bzd::forward<Args>(args)...);
			bzd::panic();
		}
	}
}
