#pragma once

#include "cc/bzd/container/iterator/bidirectional.hh"

namespace bzd::iterator {

template <class T>
struct RandomAccessDefaultPolicies : BidirectionalDefaultPolicies<T>
{
	using ValueType = T;
	using IndexType = bzd::Size;
	using DifferenceType = bzd::Int32;
	using BidirectionalDefaultPolicies<T>::increment;
	using BidirectionalDefaultPolicies<T>::decrement;

	static constexpr void increment(T*& data, const int n) noexcept { data += n; }
	static constexpr void decrement(T*& data, const int n) noexcept { data -= n; }
	static constexpr auto& at(auto* data, const Size n) noexcept { return data[n]; }
};

template <class T, class CRTP = void, class Policies = RandomAccessDefaultPolicies<T>>
class RandomAccess : public Bidirectional<T, CRTP, Policies>
{
public: // Traits
	using Self = RandomAccess<T, CRTP, Policies>;
	using ActualSelf = typeTraits::Conditional<typeTraits::isSame<CRTP, void>, Self, CRTP>;
	using Parent = Bidirectional<T, CRTP, Policies>;
	using Category = typeTraits::RandomAccessTag;
	using IndexType = bzd::Size;
	using DifferenceType = bzd::Int32;
	using ValueType = T;

public: // Constructors
	constexpr RandomAccess(ValueType* data) noexcept : Parent{data} {}

public: // Copy/move constructors/assignments
	RandomAccess(const Self&) = default;
	Self& operator=(const Self&) = default;
	RandomAccess(Self&&) = default;
	Self& operator=(Self&&) = default;

public: // API
	[[nodiscard]] constexpr ActualSelf operator+(const int n) const noexcept
	{
		ActualSelf it{static_cast<const ActualSelf&>(*this)};
		Policies::increment(it.data_, n);
		return it;
	}

	[[nodiscard]] constexpr ActualSelf operator-(const int n) const noexcept
	{
		ActualSelf it(static_cast<const ActualSelf&>(*this));
		Policies::decrement(it.data_, n);
		return it;
	}

	[[nodiscard]] constexpr DifferenceType operator-(const Self& other) const noexcept
	{
		return static_cast<DifferenceType>(&(this->operator*()) - &(*other));
	}

	constexpr ActualSelf& operator+=(const int n) noexcept
	{
		Policies::increment(this->data_, n);
		return static_cast<ActualSelf&>(*this);
	}

	constexpr ActualSelf& operator-=(const int n) noexcept
	{
		Policies::decrement(this->data_, n);
		return static_cast<ActualSelf&>(*this);
	}

	[[nodiscard]] constexpr auto& operator[](const Size index) noexcept { return Policies::at(this->data_, index); }
	[[nodiscard]] constexpr auto& operator[](const Size index) const noexcept { return Policies::at(this->data_, index); }
};
} // namespace bzd::iterator
