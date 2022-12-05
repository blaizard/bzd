#pragma once

#include "cc/bzd/container/iterator/random_access.hh"

namespace bzd::iterator {

template <class T, class CRTP = void, class Policies = impl::DefaultPolicies<T>>
class Contiguous : public impl::ClassTraits<Contiguous, T, CRTP, Policies, RandomAccess>::Parent
{
private:
	using Traits = impl::ClassTraits<Contiguous, T, CRTP, Policies, RandomAccess>;

public: // Traits
	using Self = typename Traits::Self;
	using Parent = typename Traits::Parent;
	using Category = typeTraits::ContiguousTag;
	using IndexType = typename Policies::IndexType;
	using DifferenceType = typename Policies::DifferenceType;
	using ValueType = typename Policies::ValueType;

public: // Constructors
	constexpr Contiguous(ValueType* data) noexcept : Parent{data} {}

public: // Copy/move constructors/assignments
	Contiguous(const Contiguous&) = default;
	Contiguous& operator=(const Contiguous&) = default;
	Contiguous(Contiguous&&) = default;
	Contiguous& operator=(Contiguous&&) = default;
	~Contiguous() = default;
};

} // namespace bzd::iterator
