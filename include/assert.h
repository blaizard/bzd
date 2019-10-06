#pragma once

#include "include/format.h"
#include "include/format.h"

namespace bzd
{
	template <class... Args>
	static constexpr void assertTrue(const bool condition, Args&&... args) noexcept
	{
		if (!condition)
		{
			bzd::String<128> message;
			bzd::format::toString(message, bzd::forward<Args>(args)...);
			while (true) {};
		}
	}
}
