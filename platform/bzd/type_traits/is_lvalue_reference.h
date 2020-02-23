#pragma once

#include "bzd/type_traits/false_type.h"
#include "bzd/type_traits/true_type.h"

namespace bzd { namespace typeTraits {
namespace impl {
template <class T>
struct IsLValueReference : public FalseType
{
};

template <class T>
struct IsLValueReference<T&> : public TrueType
{
};
} // namespace impl

template <class T>
using IsLValueReference = typename impl::IsLValueReference<T>;

template <class T>
constexpr bool isLValueReference = IsLValueReference<T>::value;

}} // namespace bzd::typeTraits
