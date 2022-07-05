#pragma once

#include "cc/bzd/container/iterator/random_access.hh"

namespace bzd::iterator {

template <class T>
class Contiguous : public RandomAccess<T, Contiguous<T>>
{
public: // Traits
	using Self = Contiguous<T>;
	using Parent = RandomAccess<T, Contiguous<T>>;
	using Category = typeTraits::ContiguousTag;
	using ValueType = T;

public: // Constructors
	constexpr Contiguous(ValueType* data) noexcept : Parent{data} {}

public: // Copy/move constructors/assignments
	Contiguous(const Self&) = default;
	Self& operator=(const Self&) = default;
	Contiguous(Self&&) = default;
	Self& operator=(Self&&) = default;
};

} // namespace bzd::iterator
