#pragma once

#include "cc/bzd/type_traits/is_lvalue_reference.h"
#include "cc/bzd/type_traits/remove_reference.h"

namespace bzd {
template <class T>
constexpr T&& forward(typeTraits::RemoveReference<T>& t) noexcept
{
	return static_cast<T&&>(t);
}

template <class T>
constexpr T&& forward(typeTraits::RemoveReference<T>&& t) noexcept
{
	static_assert(!bzd::typeTraits::isLValueReference<T>, "template argument substituting T is an lvalue reference type");
	return static_cast<T&&>(t);
}
} // namespace bzd
