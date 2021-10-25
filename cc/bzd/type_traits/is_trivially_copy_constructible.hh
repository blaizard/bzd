#pragma once

#include <type_traits>

namespace bzd::typeTraits {
template <class T>
using IsTriviallyCopyConstructible = ::std::is_trivially_copy_constructible<T>;

template <class T>
constexpr bool isTriviallyCopyConstructible = IsTriviallyCopyConstructible<T>::value;

} // namespace bzd::typeTraits
