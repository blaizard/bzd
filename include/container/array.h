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
		constexpr explicit Array(Args&&... args) noexcept
				: Span<T>(data_, N)
				, data_{args...}
		{
		}

		constexpr SizeType capacity() const noexcept
		{
			return N;
		}

	protected:
		T data_[N];
	};
}
