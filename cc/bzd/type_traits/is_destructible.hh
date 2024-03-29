#pragma once

#include "cc/bzd/type_traits/declval.hh"
#include "cc/bzd/type_traits/false_type.hh"
#include "cc/bzd/type_traits/integral_constant.hh"
#include "cc/bzd/type_traits/true_type.hh"

namespace bzd::typeTraits::impl {
struct IsDestructibleHelper
{
	template <class T, class = decltype(bzd::typeTraits::declval<T&>().~T())>
	static char test(int);
	template <class>
	static long int test(...);
};

template <class T>
struct IsDestructible : public bzd::typeTraits::IntegralConstant<bool, (sizeof(impl::IsDestructibleHelper::test<T>(0)) == sizeof(char))>
{
};

template <>
struct IsDestructible<void> : public FalseType
{
};
template <>
struct IsDestructible<void const> : public FalseType
{
};
template <>
struct IsDestructible<void volatile> : public FalseType
{
};
template <>
struct IsDestructible<void const volatile> : public FalseType
{
};
template <class T>
struct IsDestructible<T&> : public IsDestructible<T>
{
};
template <class T>
struct IsDestructible<T&&> : public IsDestructible<T>
{
};
template <class T, unsigned long int N>
struct IsDestructible<T[N]> : public IsDestructible<T>
{
};
template <class T>
struct IsDestructible<T[]> : public IsDestructible<T>
{
};
} // namespace bzd::typeTraits::impl
namespace bzd::typeTraits {
template <class T>
using IsDestructible = typename impl::IsDestructible<T>;

template <class T>
inline constexpr bool isDestructible = IsDestructible<T>::value;

} // namespace bzd::typeTraits
