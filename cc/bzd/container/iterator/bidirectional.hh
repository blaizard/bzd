#pragma once

#include "cc/bzd/container/iterator/forward.hh"

namespace bzd::iterator {

template <class T>
struct BidirectionalDefaultPolicies : ForwardDefaultPolicies<T>
{
	using ValueType = T;
	using IndexType = bzd::Size;
	using DifferenceType = bzd::Int32;

	static constexpr void decrement(T*& data) noexcept { --data; }
};

template <class T, class CRTP = void, class Policies = BidirectionalDefaultPolicies<T>>
class Bidirectional : public Forward<T, CRTP, Policies>
{
public: // Traits
	using Self = Bidirectional<T, CRTP, Policies>;
	using ActualSelf = typeTraits::Conditional<typeTraits::isSame<CRTP, void>, Self, CRTP>;
	using Parent = Forward<T, CRTP, Policies>;
	using Category = typeTraits::BidirectionalTag;
	using IndexType = typename Policies::IndexType;
	using DifferenceType = typename Policies::DifferenceType;
	using ValueType = typename Policies::ValueType;

public: // Constructors
	constexpr Bidirectional(ValueType* data) noexcept : Parent{data} {}

public: // Copy/move constructors/assignments
	Bidirectional(const Self&) = default;
	Self& operator=(const Self&) = default;
	Bidirectional(Self&&) = default;
	Self& operator=(Self&&) = default;

public: // API
	constexpr ActualSelf& operator--() noexcept
	{
		Policies::decrement(this->data_);
		return static_cast<ActualSelf&>(*this);
	}

	constexpr ActualSelf operator--(int) noexcept
	{
		ActualSelf it{static_cast<ActualSelf&>(*this)};
		Policies::decrement(this->data_);
		return it;
	}
};
} // namespace bzd::iterator
