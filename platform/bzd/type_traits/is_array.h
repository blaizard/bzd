#pragma once

#include "bzd/type_traits/false_type.h"
#include "bzd/type_traits/true_type.h"

namespace bzd { namespace typeTraits { namespace impl {
template <class T>
struct IsArray : bzd::typeTraits::falseType
{
};
template <class T>
struct IsArray<T[]> : bzd::typeTraits::trueType
{
};
template <class T, unsigned long int N>
struct IsArray<T[N]> : bzd::typeTraits::trueType
{
};
}

template <class T>
using IsArray = typename impl::IsArray<T>;

template <class T>
constexpr bool isArray = IsArray<T>::value;

}} // namespace bzd::typeTraits
