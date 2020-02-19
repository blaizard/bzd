#pragma once

#include "bzd/types.h"

namespace bzd {
constexpr void memcpy(void* dest, const void* src, const SizeType size)
{
	for (SizeType i = 0; i < size; ++i)
	{
		static_cast<char*>(dest)[i] = static_cast<const char*>(src)[i];
	}
}

constexpr void memcpy(char* dest, const char* src, const SizeType size)
{
	for (SizeType i = 0; i < size; ++i)
	{
		dest[i] = src[i];
	}
}
} // namespace bzd
