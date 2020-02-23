#pragma once

#include "bzd/type_traits/declval.h"
#include "bzd/type_traits/integral_constant.h"
#include "bzd/type_traits/void_type.h"

namespace bzd { namespace typeTraits {
namespace impl {
template <class, class T, class... Args>
struct IsConstructible : public FalseType
{
};

template <class T, class... Args>
struct IsConstructible<VoidType<decltype(T(declval<Args>()...))>, T, Args...> : public TrueType
{
};
} // namespace impl

template <class T, class... Args>
using IsConstructible = typename impl::IsConstructible<VoidType<>, T, Args...>;

template <class T, class... Args>
constexpr bool isConstructible = IsConstructible<T, Args...>::value;

}} // namespace bzd::typeTraits
