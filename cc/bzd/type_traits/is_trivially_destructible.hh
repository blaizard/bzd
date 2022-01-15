#pragma once

#include "cc/bzd/type_traits/integral_constant.hh"
#include "cc/bzd/type_traits/is_destructible.hh"

#ifndef __is_identifier
#define __is_identifier(__x) 1
#endif
// NOLINTNEXTLINE(bugprone-reserved-identifier)
#define __has_keyword(__x) !(__is_identifier(__x))
#ifndef __has_feature
#define __has_feature(__x) 0
#endif

namespace bzd::typeTraits::impl {
#if __has_keyword(__is_trivially_destructible)
template <class T>
struct IsTriviallyDestructible : public bzd::typeTraits::IntegralConstant<bool, __is_trivially_destructible(T)>
{
};
#elif __has_feature(has_trivial_destructor) || defined(__GNUC__)
template <class T>
struct IsTriviallyDestructible : public bzd::typeTraits::IntegralConstant<bool, isDestructible<T>&& __has_trivial_destructor(T)>
{
};
#else
static_assert(false, "Unsupported compiler");
#endif
} // namespace bzd::typeTraits::impl

namespace bzd::typeTraits {
template <class T>
using IsTriviallyDestructible = typename impl::IsTriviallyDestructible<T>;

template <class T>
constexpr bool isTriviallyDestructible = IsTriviallyDestructible<T>::value;

} // namespace bzd::typeTraits
