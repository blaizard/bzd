#pragma once

#include "bzd/type_traits/declval.h"
#include "bzd/type_traits/integral_constant.h"
#include "bzd/type_traits/void_type.h"

namespace bzd { namespace typeTraits {
namespace impl {
template <class, class T, class... Args>
struct IsConstructible : public falseType
{
};

template <class T, class... Args>
struct IsConstructible<voidType<decltype(T(declval<Args>()...))>, T, Args...> : public trueType
{
};
} // namespace impl

template <class T, class... Args>
using IsConstructible = typename impl::IsConstructible<voidType<>, T, Args...>;

template <class T, class... Args>
constexpr bool isConstructible = IsConstructible<T, Args...>::value;

}} // namespace bzd::typeTraits
