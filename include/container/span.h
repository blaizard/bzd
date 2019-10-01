#pragma once

#include "include/types.h"
#include "include/type_traits/utils.h"
#include "include/type_traits/const_volatile.h"

namespace bzd
{
	template <class T>
	class Span
	{
	protected:
		using DataType = T;
		using SelfType = Span<DataType>;
		using IsConst = typename bzd::typeTraits::isConst<DataType>;

	public:
		static constexpr const SizeType npos = static_cast<SizeType>(-1);

		class Iterator
		{
		public:
			constexpr Iterator(SelfType& span, const SizeType index)
				: span_(&span), index_(index)
			{
			}

			constexpr Iterator& operator++() noexcept
			{
				++index_;
				return *this;
			}

			constexpr Iterator& operator--() noexcept
			{
				--index_;
				return *this;
			}

			constexpr Iterator operator-(const int n) const noexcept
			{
				Iterator it(*this);
				it.index_ -= n;
				return it;
			}

			constexpr Iterator operator+(const int n) const noexcept
			{
				Iterator it(*this);
				it.index_ += n;
				return it;
			}

			constexpr Iterator& operator-=(const int n) noexcept
			{
				index_ -= n;
				return *this;
			}

			constexpr Iterator& operator+=(const int n) noexcept
			{
				index_ += n;
				return *this;
			}

			constexpr bool operator==(const Iterator& it) const noexcept
			{
				return it.index_ == index_;
			}

			constexpr bool operator!=(const Iterator& it) const noexcept
			{
				return !(it == *this);
			}

			constexpr DataType& operator*()
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
			constexpr ConstIterator(const SelfType& span, const SizeType index)
				: span_(&span), index_(index)
			{
			}

			constexpr ConstIterator& operator++() noexcept
			{
				++index_;
				return *this;
			}

			constexpr ConstIterator& operator--() noexcept
			{
				--index_;
				return *this;
			}

			constexpr ConstIterator operator-(const int n) const noexcept
			{
				ConstIterator it(*this);
				it.index_ -= n;
				return it;
			}

			constexpr ConstIterator operator+(const int n) const noexcept
			{
				ConstIterator it(*this);
				it.index_ += n;
				return it;
			}

			constexpr ConstIterator& operator-=(const int n) noexcept
			{
				index_ -= n;
				return *this;
			}

			constexpr ConstIterator& operator+=(const int n) noexcept
			{
				index_ += n;
				return *this;
			}

			constexpr bool operator==(const ConstIterator& it) const noexcept
			{
				return it.index_ == index_;
			}

			constexpr bool operator!=(const ConstIterator& it) const noexcept
			{
				return !(it == *this);
			}

			constexpr const DataType& operator*() const
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
		constexpr Span(T* const data, const SizeType size) noexcept
				: data_(data), size_(size)
		{
		}

		template<class Q = Iterator>
		constexpr typename bzd::typeTraits::enableIf<!IsConst::value, Q>::type begin() noexcept
		{
			return Iterator(*this, 0);
		}

		template<class Q = Iterator>
		constexpr typename bzd::typeTraits::enableIf<!IsConst::value, Q>::type end() noexcept
		{
			return Iterator(*this, size());
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

		template<class Q = DataType>
		constexpr typename bzd::typeTraits::enableIf<!IsConst::value, Q&>::type operator[](const SizeType index)
		{
			return data_[index];
		}

		constexpr const DataType& operator[](const SizeType index) const
		{
			return data_[index];
		}

		// at

		template<class Q = DataType>
		constexpr typename bzd::typeTraits::enableIf<!IsConst::value, Q&>::type at(const SizeType index)
		{
			return data_[index];
		}

		constexpr const T& at(const SizeType index) const
		{
			return data_[index];
		}

		// front

		template<class Q = DataType>
		constexpr typename bzd::typeTraits::enableIf<!IsConst::value, Q&>::type front() noexcept { return data_[0]; }
		constexpr const DataType& front() const noexcept { return data_[0]; }

		// back

		template<class Q = DataType>
		constexpr typename bzd::typeTraits::enableIf<!IsConst::value, Q&>::type back() noexcept { return data_[size_ - 1]; }
		constexpr const DataType& back() const noexcept { return data_[size_ - 1]; }

		template<class Q = DataType>
		constexpr typename bzd::typeTraits::enableIf<!IsConst::value, Q*>::type data() noexcept
		{
			return data_;
		}

		constexpr const DataType* data() const noexcept
		{
			return data_;
		}

		constexpr SizeType find(const DataType& item, const SizeType start = 0) const noexcept
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

		constexpr bool empty() const noexcept
		{
			return (size_ == 0);
		}

	protected:
		T* data_;
		SizeType size_;
	};
}
