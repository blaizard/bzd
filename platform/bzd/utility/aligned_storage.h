#pragma once

#include "bzd/types.h"

namespace bzd {
namespace impl {
template <SizeType Len, SizeType Align>
struct AlignedStorage
{
	struct type
	{
		alignas(Align) unsigned char data[Len];
	};
};
} // namespace impl

template <SizeType Len, SizeType Align>
using AlignedStorage = typename impl::AlignedStorage<Len, Align>::type;

} // namespace bzd
