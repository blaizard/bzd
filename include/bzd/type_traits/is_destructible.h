#pragma once

#include "bzd/type_traits/declval.h"
#include "bzd/type_traits/false_type.h"
#include "bzd/type_traits/integral_constant.h"
#include "bzd/type_traits/true_type.h"

namespace bzd { namespace typeTraits {
namespace impl {
struct isDestructible
{
	template <class T, class = decltype(bzd::typeTraits::declval<T &>().~T())>
	static char test(int);
	template <class>
	static long int test(...);
};
}  // namespace impl

template <class T>
struct isDestructible : public bzd::typeTraits::integralConstant<bool, (sizeof(impl::isDestructible::test<T>(0)) == sizeof(char))>
{
};

template <>
struct isDestructible<void> : public falseType
{
};
template <>
struct isDestructible<void const> : public falseType
{
};
template <>
struct isDestructible<void volatile> : public falseType
{
};
template <>
struct isDestructible<void const volatile> : public falseType
{
};
template <class T>
struct isDestructible<T &> : public isDestructible<T>
{
};
template <class T>
struct isDestructible<T &&> : public isDestructible<T>
{
};
template <class T, unsigned long int N>
struct isDestructible<T[N]> : public isDestructible<T>
{
};
template <class T>
struct isDestructible<T[]> : public isDestructible<T>
{
};
}}	// namespace bzd::typeTraits
