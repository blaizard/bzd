#pragma once

#include "cc/bzd/container/iterator/random_access.hh"

namespace bzd::iterator {

template <class ValueType>
class Contiguous : public RandomAccess<ValueType, Contiguous<ValueType>>
{
public: // Traits
	using Self = Contiguous<ValueType>;
	using Parent = RandomAccess<ValueType, Self>;
	using Category = typeTraits::ContiguousTag;

public: // Constructors
	using Parent::Parent;

public: // Copy/move constructors/assignments
	constexpr Contiguous(const Self&) noexcept = default;
	constexpr Self& operator=(const Self&) noexcept = default;
	constexpr Contiguous(Self&&) noexcept = default;
	constexpr Self& operator=(Self&&) noexcept = default;
};

} // namespace bzd::iterator
