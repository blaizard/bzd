#pragma once

#include "cc/bzd/container/iterator/forward.hh"

namespace bzd::iterator {

template <class T, class CRTP = void, class Policies = impl::DefaultPolicies<T>>
class Bidirectional : public impl::ClassTraits<Bidirectional, T, CRTP, Policies, Forward>::Parent
{
private:
	using Traits = impl::ClassTraits<Bidirectional, T, CRTP, Policies, Forward>;

public: // Traits
	using Self = typename Traits::Self;
	using ActualSelf = typename Traits::ActualSelf;
	using Parent = typename Traits::Parent;
	using Category = typeTraits::BidirectionalTag;
	using IndexType = typename Policies::IndexType;
	using DifferenceType = typename Policies::DifferenceType;
	using ValueType = typename Policies::ValueType;

public: // Constructors
	constexpr Bidirectional(ValueType* data) noexcept : Parent{data} {}

public: // Copy/move constructors/assignments
	Bidirectional(const Bidirectional&) = default;
	Bidirectional& operator=(const Bidirectional&) = default;
	Bidirectional(Bidirectional&&) = default;
	Bidirectional& operator=(Bidirectional&&) = default;
	~Bidirectional() = default;

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
