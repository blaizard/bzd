#pragma once

#include "bzd/utility/offset_of.h"

namespace bzd
{
	template <class T, class M>
	static inline constexpr T* containerOf(M* ptr, const M T::*member)
	{
		return reinterpret_cast<T*>(reinterpret_cast<IntPtrType>(ptr) - offsetOf(member));
	}
}
