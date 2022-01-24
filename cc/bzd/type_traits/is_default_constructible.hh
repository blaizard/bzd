#pragma once

#include "cc/bzd/type_traits/false_type.hh"
#include "cc/bzd/type_traits/true_type.hh"
#include "cc/bzd/type_traits/void_type.hh"

namespace bzd::typeTraits::impl {

template <class T, class = void>
struct IsDefaultConstructible : bzd::typeTraits::FalseType
{
};

template <class T>
struct IsDefaultConstructible<T, bzd::typeTraits::VoidType<decltype(T())>> : bzd::typeTraits::TrueType
{
};

} // namespace bzd::typeTraits::impl

namespace bzd::typeTraits {

template <class T>
using IsDefaultConstructible = typename impl::IsDefaultConstructible<T>;

template <class T>
inline constexpr bool isDefaultConstructible = IsDefaultConstructible<T>::value;

} // namespace bzd::typeTraits
