#pragma once

#include "bzd/type_traits/integral_constant.h"
#include "bzd/type_traits/is_lvalue_reference.h"
#include "bzd/type_traits/is_rvalue_reference.h"

namespace bzd::typeTraits::impl {
template <class T>
struct IsReference : public IntegralConstant<bool, isLValueReference<T> || isRValueReference<T>>
{
};
} // namespace bzd::typeTraits::impl

namespace bzd::typeTraits {
template <class T>
using IsReference = typename impl::IsReference<T>;

template <class T>
constexpr bool isReference = IsReference<T>::value;

} // namespace bzd::typeTraits
