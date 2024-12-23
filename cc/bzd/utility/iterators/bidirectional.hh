#pragma once

#include "cc/bzd/utility/iterators/forward.hh"

namespace bzd::iterator {

template <class T, class CRTP = void, class Policies = impl::DefaultPolicies<T>>
class Bidirectional : public impl::ClassTraits<Bidirectional, T, CRTP, Policies, Forward>::Parent
{
private:
	using Traits = impl::ClassTraits<Bidirectional, T, CRTP, Policies, Forward>;
	using StorageValueType = typename Policies::StorageValueType;

public: // Traits
	using Self = typename Traits::Self;
	using ActualSelf = typename Traits::ActualSelf;
	using Parent = typename Traits::Parent;
	using DifferenceType = typename Policies::DifferenceType;
	using ValueType = typename Policies::ValueType;
	static constexpr auto category = typeTraits::IteratorCategory::bidirectional;

public: // Constructors
	constexpr Bidirectional(StorageValueType* data) noexcept : Parent{data} {}

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
