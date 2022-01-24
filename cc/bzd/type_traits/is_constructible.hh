#pragma once

#include "cc/bzd/type_traits/declval.hh"
#include "cc/bzd/type_traits/integral_constant.hh"
#include "cc/bzd/type_traits/void_type.hh"

namespace bzd::typeTraits::impl {
template <class, class T, class... Args>
struct IsConstructible : public FalseType
{
};

template <class T, class... Args>
struct IsConstructible<VoidType<decltype(T(declval<Args>()...))>, T, Args...> : public TrueType
{
};
} // namespace bzd::typeTraits::impl
namespace bzd::typeTraits {
template <class T, class... Args>
using IsConstructible = typename impl::IsConstructible<VoidType<>, T, Args...>;

template <class T, class U>
using IsConstructible1 = typename impl::IsConstructible<VoidType<>, T, U>;

template <class T, class... Args>
inline constexpr bool isConstructible = IsConstructible<T, Args...>::value;

} // namespace bzd::typeTraits
