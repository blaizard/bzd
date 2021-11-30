#pragma once

#include "cc/bzd/type_traits/conditional.hh"
#include "cc/bzd/type_traits/is_const.hh"
#include "cc/bzd/type_traits/is_lvalue_reference.hh"
#include "cc/bzd/type_traits/is_rvalue_reference.hh"
#include "cc/bzd/type_traits/is_volatile.hh"
#include "cc/bzd/type_traits/remove_cvref.hh"
#include "cc/bzd/utility/forward.hh"

namespace bzd {

template <class T, class U>
using MirrorRef =
	typeTraits::Conditional<typeTraits::isLValueReference<U>, T&, typeTraits::Conditional<typeTraits::isRValueReference<U>, T&&, T>>;

template <class T, class U>
using MirrorConst = typeTraits::Conditional<typeTraits::isConst<U>, const T, T>;

template <class T, class U>
using MirrorVolatile = typeTraits::Conditional<typeTraits::isVolatile<U>, volatile T, T>;

template <class T, class U>
using MirrorCVRef = MirrorConst<MirrorVolatile<MirrorRef<typeTraits::RemoveCVRef<T>, U>, U>, U>;

template <class T, class U, class V = MirrorCVRef<T, U>>
constexpr V&& forward_cast(typeTraits::RemoveReference<U>&& u) noexcept
{
	return static_cast<V&&>(u);
}

template <class T, class U, class V = MirrorCVRef<T, U>>
constexpr V&& forward_cast(typeTraits::RemoveReference<U>& u) noexcept
{
	return static_cast<V&&>(u);
}

} // namespace bzd
