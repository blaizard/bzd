#pragma once

#include "bzd/type_traits/false_type.h"
#include "bzd/type_traits/integral_constant.h"
#include "bzd/type_traits/is_union.h"

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

/*
namespace detail {
template <class T>
std::integral_constant<bool, !std::is_union<T>::value> test(int T::*);

template <class>
std::false_type test(...);
}

template <class T>
struct is_class : decltype(detail::test<T>(nullptr))
{};
*/