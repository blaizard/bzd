#pragma once

#include "include/types.h"
#include "include/container/span.h"

namespace bzd
{
	namespace impl
	{
		template <class T, class Impl>
		class Vector : public Impl
		{
		protected:
			using Impl::size_;
			using Impl::data_;

		public:
			template <class... Args>
			Vector(const SizeType capacity, Args&&... args)
					: Impl(args...), capacity_(capacity)
			{
			}

			void pushBack(const T& element) noexcept
			{
				if (size_ < capacity_)
				{
					data_[size_] = element;
					++size_;
				}
			}

			SizeType capacity() const noexcept
			{
				return capacity_;
			}

			void clear() noexcept
			{
				resize(0);
			}

			void resize(const size_t n) noexcept
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
		using Vector = impl::Vector<T, Span<T, /*IsSizeConst*/false>>;
	}

	template <class T, SizeType N>
	class Vector : public interface::Vector<T>
	{
	public:
		template <class... Args>
		Vector(Args&&... args)
				: interface::Vector<T>(N, data_, sizeof...(Args))
				, data_{args...}
		{
		}

	protected:
		T data_[N];
	};
}
