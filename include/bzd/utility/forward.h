#pragma once

#include "bzd/type_traits/remove_reference.h"
#include "bzd/type_traits/is_lvalue_reference.h"

namespace bzd
{
	template <class T>
	constexpr T&& forward(typename typeTraits::removeReference<T>::type& t) noexcept
	{
		return static_cast<T&&>(t);
	}

	template <class T>
	constexpr T&& forward(typename typeTraits::removeReference<T>::type&& t) noexcept
	{
		static_assert(!bzd::typeTraits::isLValueReference<T>::value, "template argument substituting T is an lvalue reference type");
		return static_cast<T&&>(t);
	}
}
