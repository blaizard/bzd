#pragma once

#include "cc/bzd/type_traits/underlying_type.hh"

namespace bzd {
template <class T>
constexpr typeTraits::UnderlyingType<T> toUnderlying(const T value) noexcept
{
	return static_cast<typeTraits::UnderlyingType<T>>(value);
}
} // namespace bzd
