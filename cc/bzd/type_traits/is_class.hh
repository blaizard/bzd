#pragma once

#include "cc/bzd/type_traits/false_type.hh"
#include "cc/bzd/type_traits/integral_constant.hh"
#include "cc/bzd/type_traits/is_union.hh"

namespace bzd::typeTraits::impl {
template <class T>
bzd::typeTraits::IntegralConstant<bool, !isUnion<T>> IsClass(int T::*);

template <class>
bzd::typeTraits::FalseType IsClass(...);
} // namespace bzd::typeTraits::impl

namespace bzd::typeTraits {
template <class T>
struct IsClass : decltype(impl::IsClass<T>(nullptr))
{
};

template <class T>
constexpr bool isClass = IsClass<T>::value;
} // namespace bzd::typeTraits
