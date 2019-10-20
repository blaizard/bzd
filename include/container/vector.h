#pragma once

#include "include/types.h"
#include "include/container/span.h"
#include "include/utility.h"

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

			constexpr void pushBack(const T& element) noexcept
			{
				if (size_ < capacity_)
				{
					Parent::at(size_) = element;
					++size_;
				}
			}

			constexpr SizeType capacity() const noexcept
			{
				return capacity_;
			}

			constexpr void clear() noexcept
			{
				resize(0);
			}

			constexpr void resize(const size_t n) noexcept
			{
				size_ = (n < capacity_) ? n : capacity_;
			}

		public:
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
