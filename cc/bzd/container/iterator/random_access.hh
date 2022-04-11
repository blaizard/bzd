#pragma once

#include "cc/bzd/platform/types.hh"
#include "cc/bzd/type_traits/conditional.hh"
#include "cc/bzd/type_traits/is_same.hh"
#include "cc/bzd/type_traits/iterator.hh"

namespace bzd::iterator {

template <class T, class CRTP = void>
class RandomAccess : public typeTraits::IteratorBase
{
public: // Traits
	using Self = RandomAccess<T, CRTP>;
	using ActualSelf = typeTraits::Conditional<typeTraits::isSame<CRTP, void>, Self, CRTP>;
	using Category = typeTraits::RandomAccessTag;
	using IndexType = bzd::SizeType;
	using DifferenceType = bzd::Int32Type;
	using ValueType = T;

public: // Constructors
	constexpr RandomAccess(ValueType* data) : data_{data} {}

public: // Copy/move constructors/assignments
	constexpr RandomAccess(const Self&) noexcept = default;
	constexpr Self& operator=(const Self&) noexcept = default;
	constexpr RandomAccess(Self&&) noexcept = default;
	constexpr Self& operator=(Self&&) noexcept = default;

public: // API
	constexpr ActualSelf& operator++() noexcept
	{
		++data_;
		return static_cast<ActualSelf&>(*this);
	}

	constexpr ActualSelf operator++(int) noexcept
	{
		ActualSelf it{static_cast<const ActualSelf&>(*this)};
		++data_;
		return it;
	}

	constexpr ActualSelf& operator--() noexcept
	{
		--data_;
		return static_cast<ActualSelf&>(*this);
	}

	constexpr ActualSelf operator--(int) noexcept
	{
		ActualSelf it{static_cast<ActualSelf&>(*this)};
		--data_;
		return it;
	}

	[[nodiscard]] constexpr ActualSelf operator-(const int n) const noexcept
	{
		ActualSelf it(static_cast<const ActualSelf&>(*this));
		it.data_ -= n;
		return it;
	}

	[[nodiscard]] constexpr ActualSelf operator+(const int n) const noexcept
	{
		ActualSelf it{static_cast<const ActualSelf&>(*this)};
		it.data_ += n;
		return it;
	}

	[[nodiscard]] constexpr DifferenceType operator-(const Self& other) const noexcept
	{
		return static_cast<DifferenceType>(&(operator*()) - &(*other));
	}

	constexpr ActualSelf& operator-=(const int n) noexcept
	{
		data_ -= n;
		return static_cast<ActualSelf&>(*this);
	}

	constexpr ActualSelf& operator+=(const int n) noexcept
	{
		data_ += n;
		return static_cast<ActualSelf&>(*this);
	}

	[[nodiscard]] constexpr BoolType operator==(const Self& it) const noexcept { return it.data_ == data_; }

	[[nodiscard]] constexpr BoolType operator!=(const Self& it) const noexcept { return !(it == *this); }

	[[nodiscard]] constexpr ValueType& operator*() const { return *data_; }

	[[nodiscard]] constexpr ValueType* operator->() const { return data_; }

	[[nodiscard]] constexpr auto& operator[](const SizeType index) noexcept { return data_[index]; }
	[[nodiscard]] constexpr auto& operator[](const SizeType index) const noexcept { return data_[index]; }

private:
	ValueType* data_{nullptr};
};
} // namespace bzd::iterator
