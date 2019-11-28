#pragma once

#include "bzd/container/string_view.h"
#include "bzd/system.h"

namespace bzd
{
	namespace assert
	{
		constexpr void isTrue(const bool condition)
		{
			if (!condition)
			{
				bzd::panic();
			}
		}

		constexpr void isTrue(const bool condition, const bzd::StringView& message)
		{
			if (!condition)
			{
				bzd::getOut().write(message);
				bzd::panic();
			}
		}
	}
}
