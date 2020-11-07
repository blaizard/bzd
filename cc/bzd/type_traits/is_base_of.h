#pragma once

#include "bzd/type_traits/false_type.h"
#include "bzd/type_traits/integral_constant.h"
#include "bzd/type_traits/is_class.h"
#include "bzd/type_traits/true_type.h"

namespace bzd::typeTraits::impl {

template <class B>
bzd::typeTraits::TrueType IsBaseOfPtrConvertible(const volatile B*);
template <class>
bzd::typeTraits::FalseType IsBaseOfPtrConvertible(const volatile void*);

template <class, class>
auto IsBaseOf(...) -> bzd::typeTraits::TrueType;
template <class B, class D>
auto IsBaseOf(int) -> decltype(IsBaseOfPtrConvertible<B>(static_cast<D*>(nullptr)));
} // namespace bzd::typeTraits::impl

namespace bzd::typeTraits {

template <class Base, class Derived>
struct IsBaseOf
	: bzd::typeTraits::IntegralConstant<bool,
										bzd::typeTraits::isClass<Base> &&
											bzd::typeTraits::isClass<Derived>&& decltype(impl::IsBaseOf<Base, Derived>(0))::value>
{
};

template <class Base, class Derived>
constexpr bool isBaseOf = IsBaseOf<Base, Derived>::value;
} // namespace bzd::typeTraits
