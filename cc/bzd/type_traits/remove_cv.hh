#pragma once

#include "cc/bzd/type_traits/remove_const.hh"
#include "cc/bzd/type_traits/remove_volatile.hh"

namespace bzd::typeTraits {

template <class T>
using RemoveCV = RemoveVolatile<RemoveConst<T>>;

} // namespace bzd::typeTraits
