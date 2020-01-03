#pragma once

#include "bzd/types.h"

namespace bzd {
constexpr void memcpy(char *dest, const char *src, const SizeType size)
{
	for (SizeType i = 0; i < size; ++i)
	{
		dest[i] = src[i];
	}
}
}  // namespace bzd
