#pragma once

#include "bzd/types.h"
#include "bzd/container/span.h"

namespace bzd
{
	/**
	 * \brief A container that encapsulates fixed size arrays.
	 * 
	 * The container combines the performance and accessibility of a C-style array
	 * with the benefits of a standard container, such as knowing its own size, supporting
	 * assignment, random access iterators, etc.
	 * Unlike a C-style array, it doesn't decay to T* automatically.
	 */
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

		constexpr Array() noexcept
				: Span<T>(data_, N)
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
