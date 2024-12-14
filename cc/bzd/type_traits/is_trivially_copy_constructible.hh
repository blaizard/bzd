#pragma once

#include <type_traits>

namespace bzd::typeTraits {
template <class T>
using IsTriviallyCopyConstructible = ::std::is_trivially_copy_constructible<T>;

template <class T>
inline constexpr bool isTriviallyCopyConstructible = IsTriviallyCopyConstructible<T>::value;

} // namespace bzd::typeTraits

namespace bzd::concepts {

template <class T>
concept triviallyCopyConstructible = bzd::typeTraits::isTriviallyCopyConstructible<T>;

}
