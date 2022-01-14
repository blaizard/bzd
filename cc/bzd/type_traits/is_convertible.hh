#pragma once

#include "cc/bzd/platform/types.hh"
#include "cc/bzd/type_traits/declval.hh"
#include "cc/bzd/type_traits/integral_constant.hh"
#include "cc/bzd/type_traits/void_type.hh"

namespace bzd::typeTraits::impl {

template <class T>
auto testReturnable(int) -> decltype(void(static_cast<T (*)()>(nullptr)), TrueType{});

template <class>
auto testReturnable(...) -> FalseType;

template <class From, class To>
auto testImplicitlyConvertible(int) -> decltype(void(declval<void (&)(To)>()(declval<From>())), TrueType{});

template <class, class>
auto testImplicitlyConvertible(...) -> FalseType;

} // namespace bzd::typeTraits::impl
namespace bzd::typeTraits {

template <class From, class To>
struct IsConvertible
	: IntegralConstant<BoolType,
					   (decltype(impl::testReturnable<To>(0))::value&& decltype(impl::testImplicitlyConvertible<From, To>(0))::value) ||
						   (isVoid<From> && isVoid<To>)>
{
};

template <class From, class To>
constexpr bool isConvertible = IsConvertible<From, To>::value;

} // namespace bzd::typeTraits
