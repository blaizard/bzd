#pragma once

#include "cc/bzd/type_traits/integral_constant.hh"
#include "cc/bzd/type_traits/remove_cv.hh"

namespace bzd::typeTraits::impl {
template <class T>
struct IsEnum : public bzd::typeTraits::IntegralConstant<bool, __is_enum(RemoveCV<T>)>
{
};
} // namespace bzd::typeTraits::impl

namespace bzd::typeTraits {
template <class T>
using IsEnum = typename impl::IsEnum<T>;

template <class T>
inline constexpr bool isEnum = IsEnum<T>::value;
} // namespace bzd::typeTraits

namespace bzd::concepts {
template <class T>
concept isEnum = typeTraits::isEnum<T>;
}
