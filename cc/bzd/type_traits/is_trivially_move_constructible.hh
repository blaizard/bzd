#pragma once

#include <type_traits>

namespace bzd::typeTraits {
template <class T>
using IsTriviallyMoveConstructible = ::std::is_trivially_move_constructible<T>;

template <class T>
inline constexpr bool isTriviallyMoveConstructible = IsTriviallyMoveConstructible<T>::value;

} // namespace bzd::typeTraits

namespace bzd::concepts {

template <class T>
concept triviallyMoveConstructible = bzd::typeTraits::isTriviallyMoveConstructible<T>;

}
