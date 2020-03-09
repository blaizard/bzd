#pragma once

#include "bzd/types.h"
#include "bzd/container/buffer.h"
#include "bzd/utility/forward.h"

namespace bzd {

class Artifact : public ConstBuffer
{
public:
	template <class Data>
	Artifact(Data&& data, const SizeType size)
			: ConstBuffer{bzd::forward<Data>(data), size}
	{
	}
};

}
