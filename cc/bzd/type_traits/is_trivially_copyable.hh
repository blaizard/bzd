#pragma once

#include <type_traits>

namespace bzd::typeTraits {

template <class T>
using IsTriviallyCopyable = ::std::is_trivially_copyable<T>;

template <class T>
inline constexpr bool isTriviallyCopyable = IsTriviallyCopyable<T>::value;

} // namespace bzd::typeTraits
