#pragma once

#include "cc/bzd/type_traits/false_type.hh"
#include "cc/bzd/type_traits/true_type.hh"

namespace bzd::typeTraits::impl {
template <class T>
struct IsVoid : public FalseType
{
};

template <>
struct IsVoid<void> : public TrueType
{
};

template <>
struct IsVoid<const void> : public TrueType
{
};

template <>
struct IsVoid<const volatile void> : public TrueType
{
};

template <>
struct IsVoid<volatile void> : public TrueType
{
};
} // namespace bzd::typeTraits::impl

namespace bzd::typeTraits {
template <class T>
using IsVoid = typename impl::IsVoid<T>;

template <class T>
constexpr bool isVoid = IsVoid<T>::value;

} // namespace bzd::typeTraits
