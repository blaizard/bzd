#pragma once

#include "include/interface/stack.h"
#include "include/types.h"

namespace async
{
	template <const size_type N>
	class Stack : public interface::Stack
	{
	public:
		Stack()
			: interface::Stack(data_, N)
		{
		}

	private:
		byte_type data_[N];
	};
}
