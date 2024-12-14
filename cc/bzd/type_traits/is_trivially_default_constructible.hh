#pragma once

#include <type_traits>

namespace bzd::typeTraits {

template <class T>
using IsTriviallyDefaultConstructible = ::std::is_trivially_default_constructible<T>;

template <class T>
inline constexpr bool isTriviallyDefaultConstructible = IsTriviallyDefaultConstructible<T>::value;

} // namespace bzd::typeTraits

namespace bzd::concepts {

template <class T>
concept triviallyDefaultConstructible = bzd::typeTraits::isTriviallyDefaultConstructible<T>;

}
