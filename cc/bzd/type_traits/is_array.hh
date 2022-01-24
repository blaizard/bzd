#pragma once

#include "cc/bzd/type_traits/false_type.hh"
#include "cc/bzd/type_traits/true_type.hh"

namespace bzd::typeTraits::impl {
template <class T>
struct IsArray : bzd::typeTraits::FalseType
{
};
template <class T>
struct IsArray<T[]> : bzd::typeTraits::TrueType
{
};
template <class T, unsigned long int N>
struct IsArray<T[N]> : bzd::typeTraits::TrueType
{
};
} // namespace bzd::typeTraits::impl

namespace bzd::typeTraits {
template <class T>
using IsArray = typename impl::IsArray<T>;

template <class T>
inline constexpr bool isArray = IsArray<T>::value;
} // namespace bzd::typeTraits
