#pragma once

#include <type_traits>

namespace bzd::typeTraits {
template <class T>
using IsTriviallyCopyAssignable = ::std::is_trivially_copy_assignable<T>;

template <class T>
inline constexpr bool isTriviallyCopyAssignable = IsTriviallyCopyAssignable<T>::value;

} // namespace bzd::typeTraits
