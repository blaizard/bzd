#pragma once

#include "bzd/type_traits/integral_constant.h"
#include "bzd/type_traits/is_lvalue_reference.h"
#include "bzd/type_traits/is_rvalue_reference.h"

namespace bzd { namespace typeTraits {
namespace impl {
template <class T>
struct IsReference : public integralConstant<bool, isLValueReference<T> || isRValueReference<T>>
{
};
} // namespace impl

template <class T>
using IsReference = typename impl::IsReference<T>;

template <class T>
constexpr bool isReference = IsReference<T>::value;

}} // namespace bzd::typeTraits
