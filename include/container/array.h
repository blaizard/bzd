#pragma once

#include "include/types.h"
#include "include/container/span.h"

namespace bzd
{
	template <class T, SizeType N>
	class Array : public Span<T>
	{
	public:
		template <class... Args>
		Array(Args&&... args) noexcept
				: Span<T>(data_, N)
				, data_{args...}
		{
		}

		SizeType capacity() const noexcept
		{
			return N;
		}

	protected:
		T data_[N];
	};
}
