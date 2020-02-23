#pragma once

#include "bzd/type_traits/false_type.h"
#include "bzd/type_traits/true_type.h"

namespace bzd { namespace typeTraits {
namespace impl {
// Primary template
template <class>
struct IsFunction : falseType
{
};

// Specialization for regular functions
template <class Ret, class... Args>
struct IsFunction<Ret(Args...)> : trueType
{
};

// Specialization for function types that have cv-qualifiers
template <class Ret, class... Args>
struct IsFunction<Ret(Args...) const> : trueType
{
};
template <class Ret, class... Args>
struct IsFunction<Ret(Args...) volatile> : trueType
{
};
template <class Ret, class... Args>
struct IsFunction<Ret(Args...) const volatile> : trueType
{
};

// Specialization for function types that have ref-qualifiers
template <class Ret, class... Args>
struct IsFunction<Ret(Args...)&> : trueType
{
};
template <class Ret, class... Args>
struct IsFunction<Ret(Args...) const&> : trueType
{
};
template <class Ret, class... Args>
struct IsFunction<Ret(Args...) volatile&> : trueType
{
};
template <class Ret, class... Args>
struct IsFunction<Ret(Args...) const volatile&> : trueType
{
};
template <class Ret, class... Args>
struct IsFunction<Ret(Args...) &&> : trueType
{
};
template <class Ret, class... Args>
struct IsFunction<Ret(Args...) const&&> : trueType
{
};
template <class Ret, class... Args>
struct IsFunction<Ret(Args...) volatile&&> : trueType
{
};
template <class Ret, class... Args>
struct IsFunction<Ret(Args...) const volatile&&> : trueType
{
};
} // namespace impl

template <class T>
using IsFunction = typename impl::IsFunction<T>;

template <class T>
constexpr bool isFunction = IsFunction<T>::value;

}} // namespace bzd::typeTraits
