#pragma once

#include "bzd/assert_minimal.h"

#include "bzd/container/string_view.h"
#include "bzd/system.h"
#include "bzd/log.h"

namespace bzd
{
	namespace assert
	{
		template <class... Args>
		constexpr void isTrue(const bool condition, const bzd::StringView& fmt, Args&&... args)
		{
			if (!condition)
			{
				bzd::log::print(fmt, bzd::forward<Args>(args)...);
				bzd::panic();
			}
		}
	}
}
