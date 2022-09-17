#pragma once

#include <type_traits>

namespace bzd::typeTraits {

template <class T>
using IsTriviallyDestructible = ::std::is_trivially_destructible<T>;

template <class T>
inline constexpr bool isTriviallyDestructible = IsTriviallyDestructible<T>::value;

} // namespace bzd::typeTraits
