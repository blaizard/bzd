#pragma once

#include "include/types.h"
#include "include/utility.h"
#include "include/type_traits/utils.h"
#include "include/type_traits/const_volatile.h"
#include "include/container/iterator/contiguous_iterator.h"

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
		using Iterator = IteratorContiguous<Span, DataType>;
		using ConstIterator = IteratorContiguous<const Span, const DataType>;

		static constexpr const SizeType npos = static_cast<SizeType>(-1);

	public:
		constexpr Span(DataType* const data, const SizeType size) noexcept
				: data_(data), size_(size)
		{
		}

		template<class Q = IsConst, typename bzd::typeTraits::enableIf<Q::value, void>::type* = nullptr>
		constexpr Span(const Span<typename bzd::typeTraits::removeConst<DataType>::type>& span) noexcept
				: data_(span.data_), size_(span.size_)
		{
		}

		template<class Q = IsConst, typename bzd::typeTraits::enableIf<!Q::value, void>::type* = nullptr>
		constexpr Iterator begin() noexcept
		{
			return Iterator(*this, 0);
		}

		template<class Q = IsConst, typename bzd::typeTraits::enableIf<!Q::value, void>::type* = nullptr>
		constexpr Iterator end() noexcept
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

		constexpr void reverse() noexcept
		{
			SizeType counter = size_ / 2;
			while (counter--)
			{
				bzd::swap(data_[counter], data_[size_ - counter - 1]);
			}
		}

		template<class Q = IsConst, typename bzd::typeTraits::enableIf<!Q::value, void>::type* = nullptr>
		constexpr DataType& operator[](const SizeType index)
		{
			return data_[index];
		}

		constexpr const DataType& operator[](const SizeType index) const
		{
			return data_[index];
		}

		// at

		template<class Q = IsConst, typename bzd::typeTraits::enableIf<!Q::value, void>::type* = nullptr>
		constexpr DataType& at(const SizeType index)
		{
			return data_[index];
		}

		constexpr const T& at(const SizeType index) const
		{
			return data_[index];
		}

		// front

		template<class Q = IsConst, typename bzd::typeTraits::enableIf<!Q::value, void>::type* = nullptr>
		constexpr DataType& front() noexcept { return data_[0]; }
		constexpr const DataType& front() const noexcept { return data_[0]; }

		// back

		template<class Q = IsConst, typename bzd::typeTraits::enableIf<!Q::value, void>::type* = nullptr>
		constexpr DataType& back() noexcept { return data_[size_ - 1]; }
		constexpr const DataType& back() const noexcept { return data_[size_ - 1]; }

		template<class Q = IsConst, typename bzd::typeTraits::enableIf<!Q::value, void>::type* = nullptr>
		constexpr DataType* data() noexcept
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
		template <class Q>
		friend class Span;

		T* data_ = nullptr;
		SizeType size_ = 0;
	};
}
