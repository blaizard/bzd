#pragma once

#include "bzd/type_traits/remove_const.h"
#include "bzd/type_traits/remove_volatile.h"

namespace bzd { namespace typeTraits {

template <class T>
using RemoveCV = RemoveVolatile<RemoveConst<T>>;

}} // namespace bzd::typeTraits
