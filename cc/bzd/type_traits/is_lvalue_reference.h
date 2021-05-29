#pragma once

#include "cc/bzd/type_traits/false_type.h"
#include "cc/bzd/type_traits/true_type.h"

namespace bzd::typeTraits::impl {
template <class T>
struct IsLValueReference : public FalseType
{
};

template <class T>
struct IsLValueReference<T&> : public TrueType
{
};
} // namespace bzd::typeTraits::impl

namespace bzd::typeTraits {
template <class T>
using IsLValueReference = typename impl::IsLValueReference<T>;

template <class T>
constexpr bool isLValueReference = IsLValueReference<T>::value;

} // namespace bzd::typeTraits
