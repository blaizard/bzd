#pragma once

#include "cc/bzd/type_traits/false_type.h"
#include "cc/bzd/type_traits/true_type.h"

namespace bzd::typeTraits::impl {
// Primary template
template <class>
struct IsFunction : FalseType
{
};

// Specialization for regular functions
template <class Ret, class... Args>
struct IsFunction<Ret(Args...)> : TrueType
{
};

// Specialization for function types that have cv-qualifiers
template <class Ret, class... Args>
struct IsFunction<Ret(Args...) const> : TrueType
{
};
template <class Ret, class... Args>
struct IsFunction<Ret(Args...) volatile> : TrueType
{
};
template <class Ret, class... Args>
struct IsFunction<Ret(Args...) const volatile> : TrueType
{
};

// Specialization for function types that have ref-qualifiers
template <class Ret, class... Args>
struct IsFunction<Ret(Args...)&> : TrueType
{
};
template <class Ret, class... Args>
struct IsFunction<Ret(Args...) const&> : TrueType
{
};
template <class Ret, class... Args>
struct IsFunction<Ret(Args...) volatile&> : TrueType
{
};
template <class Ret, class... Args>
struct IsFunction<Ret(Args...) const volatile&> : TrueType
{
};
template <class Ret, class... Args>
struct IsFunction<Ret(Args...) &&> : TrueType
{
};
template <class Ret, class... Args>
struct IsFunction<Ret(Args...) const&&> : TrueType
{
};
template <class Ret, class... Args>
struct IsFunction<Ret(Args...) volatile&&> : TrueType
{
};
template <class Ret, class... Args>
struct IsFunction<Ret(Args...) const volatile&&> : TrueType
{
};
} // namespace bzd::typeTraits::impl

namespace bzd::typeTraits {
template <class T>
using IsFunction = typename impl::IsFunction<T>;

template <class T>
constexpr bool isFunction = IsFunction<T>::value;

} // namespace bzd::typeTraits
