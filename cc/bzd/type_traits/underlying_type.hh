#pragma once

#include <type_traits>

namespace bzd::typeTraits {
template <class T>
using UnderlyingType = std::underlying_type_t<T>;
}
