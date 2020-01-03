#pragma once

#include "bzd/type_traits/declval.h"
#include "bzd/type_traits/integral_constant.h"
#include "bzd/type_traits/void_type.h"

namespace bzd { namespace typeTraits {
namespace impl {
template <class, class T, class... Args>
struct isConstructible : public falseType
{
};

template <class T, class... Args>
struct isConstructible<voidType<decltype(T(declval<Args>()...))>, T, Args...> : public trueType
{
};
}  // namespace impl

template <class T, class... Args>
using isConstructible = impl::isConstructible<voidType<>, T, Args...>;
}}	// namespace bzd::typeTraits
