#pragma once

#include "cc/bzd/utility/iterators/random_access.hh"

namespace bzd::iterator {

template <class T, class CRTP = void, class Policies = impl::DefaultPolicies<T>>
class Contiguous : public impl::ClassTraits<Contiguous, T, CRTP, Policies, RandomAccess>::Parent
{
private:
	using Traits = impl::ClassTraits<Contiguous, T, CRTP, Policies, RandomAccess>;
	using StorageValueType = typename Policies::StorageValueType;

public: // Traits
	using Self = typename Traits::Self;
	using Parent = typename Traits::Parent;
	using DifferenceType = typename Policies::DifferenceType;
	using ValueType = typename Policies::ValueType;
	static constexpr auto category = typeTraits::IteratorCategory::contiguous;

public: // Constructors
	constexpr Contiguous(StorageValueType* data) noexcept : Parent{data} {}

public: // Copy/move constructors/assignments
	Contiguous(const Contiguous&) = default;
	Contiguous& operator=(const Contiguous&) = default;
	Contiguous(Contiguous&&) = default;
	Contiguous& operator=(Contiguous&&) = default;
	~Contiguous() = default;
};

} // namespace bzd::iterator
