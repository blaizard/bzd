#pragma once

#include "bzd/platform/types.h"

namespace bzd::iterator {
template <class DataType>
class Contiguous
{
public:
	using Self = Contiguous<DataType>;

public:
	constexpr Contiguous(DataType* const data, const SizeType index) : data_{data}, index_{index} {}

	constexpr Self& operator++() noexcept
	{
		++index_;
		return *this;
	}

	constexpr Self operator++(int) noexcept
	{
		auto it = *this;
		++index_;
		return it;
	}

	constexpr Self& operator--() noexcept
	{
		--index_;
		return *this;
	}

	constexpr Self operator--(int) noexcept
	{
		auto it = *this;
		--index_;
		return it;
	}

	constexpr Self operator-(const int n) const noexcept
	{
		Self it(*this);
		it.index_ -= n;
		return it;
	}

	constexpr Self operator+(const int n) const noexcept
	{
		Self it(*this);
		it.index_ += n;
		return it;
	}

	constexpr Self& operator-=(const int n) noexcept
	{
		index_ -= n;
		return *this;
	}

	constexpr Self& operator+=(const int n) noexcept
	{
		index_ += n;
		return *this;
	}

	constexpr bool operator==(const Self& it) const noexcept { return it.index_ == index_; }

	constexpr bool operator!=(const Self& it) const noexcept { return !(it == *this); }

	constexpr DataType& operator*() const { return data_[index_]; }

	constexpr DataType* operator->() const { return &data_[index_]; }

private:
	DataType* const data_;
	SizeType index_;
};
} // namespace bzd::iterator
