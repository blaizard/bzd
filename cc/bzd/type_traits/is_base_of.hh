#pragma once

#include "cc/bzd/type_traits/false_type.hh"
#include "cc/bzd/type_traits/integral_constant.hh"
#include "cc/bzd/type_traits/is_class.hh"
#include "cc/bzd/type_traits/true_type.hh"

namespace bzd::typeTraits::impl {

template <class B>
bzd::typeTraits::TrueType isBaseOfPtrConvertible(const volatile B*);
template <class>
bzd::typeTraits::FalseType isBaseOfPtrConvertible(const volatile void*);

template <class, class>
auto isBaseOf(...) -> bzd::typeTraits::TrueType;
template <class B, class D>
auto isBaseOf(int) -> decltype(isBaseOfPtrConvertible<B>(static_cast<D*>(nullptr)));
} // namespace bzd::typeTraits::impl

namespace bzd::typeTraits {

template <class Base, class Derived>
struct IsBaseOf
	: bzd::typeTraits::IntegralConstant<bool,
										bzd::typeTraits::isClass<Base> &&
											bzd::typeTraits::isClass<Derived>&& decltype(impl::isBaseOf<Base, Derived>(0))::value>
{
};

template <class Base, class Derived>
inline constexpr bool isBaseOf = IsBaseOf<Base, Derived>::value;
} // namespace bzd::typeTraits

namespace bzd::concepts {

template <class Base, class Derived>
concept isBaseOf = bzd::typeTraits::isBaseOf<Base, Derived>;

}
