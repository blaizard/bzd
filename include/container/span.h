#pragma once

#include "include/types.h"
#include "include/type_traits/utils.h"

namespace bzd
{
	template <class T, bool IsDataConst = false>
	class Span
	{
	protected:
		using DataType = T;
		using SelfType = Span<T, IsDataConst>;
		using DataPtrType = typename typeTraits::conditional<IsDataConst, const T*, T*>::type;

	public:
		class Iterator
		{
		public:
			Iterator(SelfType& span, const SizeType index)
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
			SelfType* span_;
			SizeType index_;
		};

		class ConstIterator
		{
		public:
			ConstIterator(const SelfType& span, const SizeType index)
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
			const SelfType* span_;
			SizeType index_;
		};

	public:
		template <class... Args>
		Span(const DataPtrType data, const SizeType size) noexcept
				: data_(data), size_(size)
		{
		}

		template<class Q = Iterator>
		typename bzd::typeTraits::enableIf<!IsDataConst, Q>::type begin() noexcept
		{
			return Iterator(*this, 0);
		}

		template<class Q = Iterator>
		typename bzd::typeTraits::enableIf<!IsDataConst, Q>::type end() noexcept
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

		template<class Q = T>
		typename bzd::typeTraits::enableIf<!IsDataConst, Q&>::type operator[](const SizeType index)
		{
			return data_[index];
		}

		const T& operator[](const SizeType index) const
		{
			return data_[index];
		}

		// front

		template<class Q = T>
		typename bzd::typeTraits::enableIf<!IsDataConst, Q&>::type front() noexcept { return data_[0]; }
		const T& front() const noexcept { return data_[0]; }

		// back

		template<class Q = T>
		typename bzd::typeTraits::enableIf<!IsDataConst, Q&>::type back() noexcept { return data_[size_ - 1]; }
		const T& back() const noexcept { return data_[size_ - 1]; }

		template<class Q = T>
		typename bzd::typeTraits::enableIf<!IsDataConst, Q*>::type data() noexcept
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
		SizeType size_;
	};
}
