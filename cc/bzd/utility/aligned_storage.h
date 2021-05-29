#pragma once

#include "cc/bzd/platform/types.h"

namespace bzd::impl {
template <SizeType Len, SizeType Align>
struct AlignedStorage
{
	struct type
	{
		alignas(Align) unsigned char data[Len];
	};
};
} // namespace bzd::impl

namespace bzd {
template <SizeType Len, SizeType Align>
using AlignedStorage = typename impl::AlignedStorage<Len, Align>::type;

} // namespace bzd
