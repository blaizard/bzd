#pragma once

#include "cc/bzd/type_traits/false_type.h"
#include "cc/bzd/type_traits/true_type.h"

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
constexpr bool isArray = IsArray<T>::value;
} // namespace bzd::typeTraits
