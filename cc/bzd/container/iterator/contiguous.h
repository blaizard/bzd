#pragma once

#include "bzd/platform/types.h"

namespace bzd::iterator {
template <class DataType>
class Contiguous
{
public:
	using SelfType = Contiguous<DataType>;

public:
	constexpr Contiguous(DataType* data, const SizeType index) : data_{data}, index_{index} {}

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

	constexpr bool operator==(const SelfType& it) const noexcept { return it.index_ == index_; }

	constexpr bool operator!=(const SelfType& it) const noexcept { return !(it == *this); }

	constexpr DataType& operator*() const { return data_[index_]; }

	constexpr DataType* operator->() const { return &data_[index_]; }

private:
	DataType* data_;
	SizeType index_;
};
} // namespace bzd::iterator
