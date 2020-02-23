#pragma once

#include "bzd/type_traits/false_type.h"
#include "bzd/type_traits/true_type.h"

namespace bzd { namespace typeTraits {
namespace impl {
template <class T>
struct IsRValueReference : public FalseType
{
};

template <class T>
struct IsRValueReference<T&&> : public TrueType
{
};
} // namespace impl

template <class T>
using IsRValueReference = typename impl::IsRValueReference<T>;

template <class T>
constexpr bool isRValueReference = IsRValueReference<T>::value;

}} // namespace bzd::typeTraits
