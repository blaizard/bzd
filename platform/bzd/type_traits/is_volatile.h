#pragma once

#include "bzd/type_traits/false_type.h"
#include "bzd/type_traits/true_type.h"

namespace bzd { namespace typeTraits {
namespace impl {
template <class T>
struct IsVolatile : falseType
{
};
template <class T>
struct IsVolatile<volatile T> : trueType
{
};
} // namespace impl

template <class T>
using IsVolatile = typename impl::IsVolatile<T>;

template <class T>
constexpr bool isVolatile = IsVolatile<T>::value;

}} // namespace bzd::typeTraits
