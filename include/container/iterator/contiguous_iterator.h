#pragma once

#include "include/types.h"

namespace bzd
{
	template <class ContainerType, class DataType>
	class IteratorContiguous
	{
	public:
		using SelfType = IteratorContiguous<ContainerType, DataType>;

	public:
		constexpr IteratorContiguous(ContainerType& container, const SizeType index)
			: container_(&container), index_(index)
		{
		}

		constexpr SelfType& operator++() noexcept
		{
			++index_;
			return *this;
		}

		constexpr SelfType operator++(int) noexcept
		{
			auto it = *this;
			++index_;
			return it;
		}

		constexpr SelfType& operator--() noexcept
		{
			--index_;
			return *this;
		}

		constexpr SelfType operator--(int) noexcept
		{
			auto it = *this;
			--index_;
			return it;
		}

		constexpr SelfType operator-(const int n) const noexcept
		{
			SelfType it(*this);
			it.index_ -= n;
			return it;
		}

		constexpr SelfType operator+(const int n) const noexcept
		{
			SelfType it(*this);
			it.index_ += n;
			return it;
		}

		constexpr SelfType& operator-=(const int n) noexcept
		{
			index_ -= n;
			return *this;
		}

		constexpr SelfType& operator+=(const int n) noexcept
		{
			index_ += n;
			return *this;
		}

		constexpr bool operator==(const SelfType& it) const noexcept
		{
			return it.index_ == index_;
		}

		constexpr bool operator!=(const SelfType& it) const noexcept
		{
			return !(it == *this);
		}

		constexpr DataType& operator*()
		{
			return (*container_)[index_];
		}

	private:
		ContainerType* container_;
		SizeType index_;
	};
}
