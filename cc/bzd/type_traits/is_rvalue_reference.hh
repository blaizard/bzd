#pragma once

#include "cc/bzd/type_traits/false_type.hh"
#include "cc/bzd/type_traits/true_type.hh"

namespace bzd::typeTraits::impl {
template <class T>
struct IsRValueReference : public FalseType
{
};

template <class T>
struct IsRValueReference<T&&> : public TrueType
{
};
} // namespace bzd::typeTraits::impl

namespace bzd::typeTraits {
template <class T>
using IsRValueReference = typename impl::IsRValueReference<T>;

template <class T>
constexpr bool isRValueReference = IsRValueReference<T>::value;

} // namespace bzd::typeTraits
