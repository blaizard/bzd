#pragma once

#include "include/types.h"
#include "include/type_traits/utils.h"

namespace bzd
{
	template <class T>
	class ConstSpan
	{
	protected:
		using DataType = T;

	public:
		static constexpr const SizeType npos = static_cast<SizeType>(-1);

		class ConstIterator
		{
		public:
			ConstIterator(const ConstSpan<T>& span, const SizeType index)
				: span_(&span), index_(index)
			{
			}

			ConstIterator& operator++() noexcept
			{
				++index_;
				return *this;
			}

			ConstIterator& operator--() noexcept
			{
				--index_;
				return *this;
			}

			ConstIterator operator-(const int n) const noexcept
			{
				ConstIterator it(*this);
				it.index_ -= n;
				return it;
			}

			ConstIterator operator+(const int n) const noexcept
			{
				ConstIterator it(*this);
				it.index_ += n;
				return it;
			}

			ConstIterator& operator-=(const int n) noexcept
			{
				index_ -= n;
				return *this;
			}

			ConstIterator& operator+=(const int n) noexcept
			{
				index_ += n;
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
				return (*span_)[index_];
			}

		private:
			// A pointer is required here in order to enable copy (operator=)
			const ConstSpan<T>* span_;
			SizeType index_;
		};

	public:
		constexpr ConstSpan(const T* data, const SizeType size) noexcept
				: data_(data), size_(size)
		{
		}

		constexpr ConstIterator begin() const noexcept
		{
			return ConstIterator(*this, 0);
		}

		constexpr ConstIterator cbegin() const noexcept
		{
			return begin();
		}

		constexpr ConstIterator end() const noexcept
		{
			return ConstIterator(*this, size());
		}

		constexpr ConstIterator cend() const noexcept
		{
			return end();
		}

		constexpr SizeType size() const noexcept
		{
			return size_;
		}

		constexpr const T& operator[](const SizeType index) const
		{
			return data_[index];
		}

		// at

		constexpr const T& at(const SizeType index) const
		{
			return data_[index];
		}

		// front

		constexpr const T& front() const noexcept { return data_[0]; }

		// back

		constexpr const T& back() const noexcept { return data_[size_ - 1]; }

		// data

		constexpr const T* data() const noexcept
		{
			return data_;
		}

		constexpr SizeType find(const T& item, const SizeType start = 0) const noexcept
		{
			for (SizeType i = start; i < size_; ++i)
			{
				if (data_[i] == item)
				{
					return i;
				}
			}
			return npos;
		}

		bool empty() const noexcept
		{
			return (size_ == 0);
		}

	protected:
		const T* data_;
		SizeType size_;
	};

	template <class T>
	class Span : public ConstSpan<T>
	{
	protected:
		using Parent = ConstSpan<T>;
		using ConstSpan<T>::data_;
		using ConstSpan<T>::size_;

	public:
		class Iterator
		{
		public:
			Iterator(Span<T>& span, const SizeType index)
				: span_(&span), index_(index)
			{
			}

			Iterator& operator++() noexcept
			{
				++index_;
				return *this;
			}

			Iterator& operator--() noexcept
			{
				--index_;
				return *this;
			}

			Iterator operator-(const int n) const noexcept
			{
				Iterator it(*this);
				it.index_ -= n;
				return it;
			}

			Iterator operator+(const int n) const noexcept
			{
				Iterator it(*this);
				it.index_ += n;
				return it;
			}

			Iterator& operator-=(const int n) noexcept
			{
				index_ -= n;
				return *this;
			}

			Iterator& operator+=(const int n) noexcept
			{
				index_ += n;
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
				return (*span_)[index_];
			}

		private:
			Span<T>* span_;
			SizeType index_;
		};

	public:
		constexpr Span(const T* data, const SizeType size) noexcept
				: ConstSpan<T>(data, size)
		{
		}

		using Parent::begin;
		Iterator begin() noexcept
		{
			return Iterator(*this, 0);
		}

		using Parent::end;
		Iterator end() noexcept
		{
			return Iterator(*this, Parent::size());
		}

		using Parent::operator[];
		T& operator[](const SizeType index)
		{
			return const_cast<T&>(data_[index]);
		}

		// at

		using Parent::at;
		T& at(const SizeType index)
		{
			return const_cast<T&>(data_[index]);
		}

		// front

		using Parent::front;
		T& front() noexcept { return at(0); }

		// back

		using Parent::back;
		T& back() noexcept { return at(size_ - 1); }

		// data

		using Parent::data;
		T* data() noexcept
		{
			return const_cast<T*>(data_);
		}
	};
}
