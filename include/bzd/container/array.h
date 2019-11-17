#pragma once

#include "bzd/types.h"
#include "bzd/container/span.h"

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

		/**
		 * \brief Returns the number of elements that the array can hold.
		 * 
		 * \return Maximum number of element this array can hold.
		 */
		constexpr SizeType capacity() const noexcept
		{
			return N;
		}

	protected:
		T data_[N];
	};
}
