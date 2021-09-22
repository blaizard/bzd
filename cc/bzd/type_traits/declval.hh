#pragma once

#include "cc/bzd/type_traits/add_rvalue_reference.hh"

namespace bzd::typeTraits {
template <class T>
AddRValueReference<T> declval();
} // namespace bzd::typeTraits
