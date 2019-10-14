#pragma once

#include "include/container/string_view.h"
#include "include/system.h"

namespace bzd
{
	constexpr void assertTrue(const bool condition)
	{
		if (!condition)
		{
			bzd::panic();
		}
	}

	constexpr void assertTrue(const bool condition, const bzd::StringView& message)
	{
		if (!condition)
		{
			getOut().write(message);
			bzd::panic();
		}
	}
}
