#pragma once

#include "bzd/type_traits/false_type.h"
#include "bzd/type_traits/remove_cv.h"
#include "bzd/type_traits/true_type.h"

namespace bzd::typeTraits::impl {

template <typename T>
struct IsPointer : public FalseType
{
};

template <typename T>
struct IsPointer<T*> : public TrueType
{
};

} // namespace bzd::typeTraits::impl

namespace bzd::typeTraits {
template <class T>
using IsPointer = typename impl::IsPointer<RemoveCV<T>>;

template <class T>
constexpr bool isPointer = IsPointer<T>::value;

} // namespace bzd::typeTraits
