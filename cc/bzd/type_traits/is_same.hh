#pragma once

#include "cc/bzd/type_traits/false_type.hh"
#include "cc/bzd/type_traits/true_type.hh"

namespace bzd::typeTraits::impl {
template <class T, class U>
struct IsSame : FalseType
{
};

template <class T>
struct IsSame<T, T> : TrueType
{
};
} // namespace bzd::typeTraits::impl

namespace bzd::typeTraits {
template <class T, class U>
using IsSame = typename impl::IsSame<T, U>;

template <class T, class U>
inline constexpr bool isSame = IsSame<T, U>::value;

} // namespace bzd::typeTraits

namespace bzd::concepts {

/// For concepts only this double equality check is needed to satisfy the same
/// atomic constraint, and therefore not create different branches if 2 concepts
/// appear with sameAs inverted. See a good answer to this question here:
/// https://stackoverflow.com/a/58511321/1107046
template <class T, class U>
concept sameAs = typeTraits::isSame<T, U> && typeTraits::isSame<U, T>;

} // namespace bzd::concepts
