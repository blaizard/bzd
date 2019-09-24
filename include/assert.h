#pragma once

namespace bzd
{
	template <class... Args>
	static inline void assertTrue(const bool condition, Args&&... args) noexcept
	{
		if (!condition)
		{
			while (true) {};
		}
	}
}
