#pragma once

#include "include/types.h"
#include "include/type_traits/utils.h"

namespace bzd
{
	template <class T, bool IsSizeConst = true, bool IsDataConst = true>
	class Span
	{
	protected:
		using DataType = T;
		using DataSizeType = typename typeTraits::conditional<IsSizeConst, const SizeType, SizeType>::type;
		using DataPtrType = typename typeTraits::conditional<IsDataConst, T* const, T*>::type;

	public:
		class Iterator
		{
		public:
			Iterator(Span<T>& span, const SizeType index)
				: span_(span), index_(index)
			{
			}

			Iterator& operator++() noexcept
			{
				++index_;
				return *this;
			}

			bool operator==(const Iterator& it) const noexcept
			{
				return it.index_ == index_;
			}

			bool operator!=(const Iterator& it) const noexcept
			{
				return !(it == *this);
			}

			T& operator*()
			{
				return span_[index_];
			}

		private:
			Span<T>& span_;
			SizeType index_;
		};

		class ConstIterator
		{
		public:
			ConstIterator(const Span<T>& span, const SizeType index)
				: span_(span), index_(index)
			{
			}

			ConstIterator& operator++() noexcept
			{
				++index_;
				return *this;
			}

			bool operator==(const ConstIterator& it) const noexcept
			{
				return it.index_ == index_;
			}

			bool operator!=(const ConstIterator& it) const noexcept
			{
				return !(it == *this);
			}

			const T& operator*() const
			{
				return span_[index_];
			}

		private:
			const Span<T>& span_;
			SizeType index_;
		};

	public:
		template <class... Args>
		Span(DataPtrType data, DataSizeType size) noexcept
				: data_(data), size_(size)
		{
		}

		Iterator begin() noexcept
		{
			return Iterator(*this, 0);
		}

		Iterator end() noexcept
		{
			return Iterator(*this, size());
		}

		ConstIterator begin() const noexcept
		{
			return ConstIterator(*this, 0);
		}

		ConstIterator cbegin() const noexcept
		{
			return begin();
		}

		ConstIterator end() const noexcept
		{
			return ConstIterator(*this, size());
		}

		ConstIterator cend() const noexcept
		{
			return end();
		}

		SizeType size() const noexcept
		{
			return size_;
		}

		T& operator[](const SizeType index)
		{
			return data_[index];
		}

		const T& operator[](const SizeType index) const
		{
			return data_[index];
		}

		T* data() noexcept
		{
			return data_;
		}

		const T* data() const noexcept
		{
			return data_;
		}

		bool empty() const noexcept
		{
			return (size_ == 0);
		}

	protected:
		DataPtrType data_;
		DataSizeType size_;
	};
}
