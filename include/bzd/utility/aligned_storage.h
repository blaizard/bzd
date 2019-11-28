#pragma once

#include "bzd/types.h"

namespace bzd
{
	template <SizeType Len, SizeType Align>
	struct alignedStorage
	{
		struct type
		{
    		alignas(Align) unsigned char data[Len];
    	};
	};
}
