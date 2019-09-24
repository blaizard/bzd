#pragma once

#include "include/interface/stack.h"
#include "include/types.h"

namespace bzd
{
	template <const SizeType N>
	class Stack : public interface::Stack
	{
	public:
		Stack()
			: interface::Stack(data_, N)
		{
		}

	private:
		ByteType data_[N];
	};
}
