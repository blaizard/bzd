#pragma once

#include "include/assert_minimal.h"

#include "include/container/string_view.h"
#include "include/utility.h"
#include "include/system.h"
#include "include/log.h"

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
