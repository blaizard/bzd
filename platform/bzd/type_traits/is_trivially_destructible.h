#pragma once

#include "bzd/type_traits/integral_constant.h"
#include "bzd/type_traits/is_destructible.h"

#ifndef __is_identifier
#define __is_identifier(__x) 1
#endif
#define __has_keyword(__x) !(__is_identifier(__x))
#ifndef __has_feature
#define __has_feature(__x) 0
#endif

namespace bzd { namespace typeTraits {
#if __has_keyword(__is_trivially_destructible)
template <class T>
struct isTriviallyDestructible : public bzd::typeTraits::integralConstant<bool, __is_trivially_destructible(T)>
{
};
#elif __has_feature(has_trivial_destructor) || defined(__GNUC__)
template <class T>
struct isTriviallyDestructible : public bzd::typeTraits::integralConstant<bool, isDestructible<T>::value&& __has_trivial_destructor(T)>
{
};
#else
static_assert(false, "Unsupported compiler");
#endif
}} // namespace bzd::typeTraits
