#pragma once

#include "bzd/type_traits/false_type.h"
#include "bzd/type_traits/true_type.h"

namespace bzd { namespace typeTraits {
namespace impl {
template <class T, class U>
struct IsSame : FalseType
{
};

template <class T>
struct IsSame<T, T> : TrueType
{
};
} // namespace impl

template <class T, class U>
using IsSame = typename impl::IsSame<T, U>;

template <class T, class U>
constexpr bool isSame = IsSame<T, U>::value;

}} // namespace bzd::typeTraits
