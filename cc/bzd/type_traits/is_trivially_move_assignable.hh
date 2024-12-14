#pragma once

#include <type_traits>

namespace bzd::typeTraits {
template <class T>
using IsTriviallyMoveAssignable = ::std::is_trivially_move_assignable<T>;

template <class T>
inline constexpr bool isTriviallyMoveAssignable = IsTriviallyMoveAssignable<T>::value;

} // namespace bzd::typeTraits

namespace bzd::concepts {

template <class T>
concept triviallyMoveAssignable = bzd::typeTraits::isTriviallyMoveAssignable<T>;

}
