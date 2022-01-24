#pragma once

#include "cc/bzd/type_traits/false_type.hh"
#include "cc/bzd/type_traits/remove_cv.hh"
#include "cc/bzd/type_traits/true_type.hh"

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
inline constexpr bool isPointer = IsPointer<T>::value;

} // namespace bzd::typeTraits
