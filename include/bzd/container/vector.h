#pragma once

#include "bzd/types.h"
#include "bzd/container/span.h"
#include "bzd/utility/forward.h"

namespace bzd
{
	namespace impl
	{
		template <class T, class Impl>
		class Vector : public Impl
		{
		protected:
			using Parent = Impl;
			using Impl::size_;
			using Impl::data_;

		public:
			template <class... Args>
			constexpr explicit Vector(const SizeType capacity, Args&&... args)
					: Impl(args...), capacity_(capacity)
			{
			}

			/**
			 * \brief Adds a new element at the end of the vector, after its current last element.
			 * 
			 * The content of val is copied (or moved) to the new element.
			 * 
			 * \param value Value to be copied (or moved) to the new element.
			 */
			constexpr void pushBack(const T& value) noexcept
			{
				if (size_ < capacity_)
				{
					Parent::at(size_) = value;
					++size_;
				}
			}

			/**
			 * \brief Returns the maximum number of elements the vector can hold.
			 * 
			 * \return Maximum number of element this vector can hold.
			 */
			constexpr SizeType capacity() const noexcept
			{
				return capacity_;
			}

			/**
			 * \brief Removes all elements.
			 */
			constexpr void clear() noexcept
			{
				resize(0);
			}

			/**
			 * \brief Change the size of the vector.
			 * 
			 * \param n The new size. Note, it must a be lower or equal to the capacity.
			 */
			constexpr void resize(const size_t n) noexcept
			{
				size_ = (n < capacity_) ? n : capacity_;
			}

		protected:
			const SizeType capacity_;
		};
	}

	namespace interface
	{
		template <class T>
		using Vector = impl::Vector<T, Span<T>>;
	}

	template <class T, SizeType N>
	class Vector : public interface::Vector<T>
	{
	public:
		template <class... Args>
		constexpr explicit Vector(Args&&... args)
				: interface::Vector<T>(N, data_, sizeof...(Args))
				, data_{bzd::forward<Args>(args)...}
		{
		}

	protected:
		T data_[N];
	};
}
