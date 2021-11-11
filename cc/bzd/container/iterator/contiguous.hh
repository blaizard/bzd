#pragma once

#include "cc/bzd/container/iterator/random_access.hh"

namespace bzd::iterator {

template <class DataType>
class Contiguous : public RandomAccess<DataType, Contiguous<DataType>>
{
public: // Traits
	using Self = Contiguous<DataType>;
	using Parent = RandomAccess<DataType, Self>;
	using Category = ContiguousTag;

public: // Constructors
	using Parent::Parent;

public: // Copy/move constructors/assignments
	constexpr Contiguous(const Self&) noexcept = default;
	constexpr Self& operator=(const Self&) noexcept = default;
	constexpr Contiguous(Self&&) noexcept = default;
	constexpr Self& operator=(Self&&) noexcept = default;
};

} // namespace bzd::iterator
