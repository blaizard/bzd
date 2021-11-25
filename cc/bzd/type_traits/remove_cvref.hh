#pragma once

#include "cc/bzd/type_traits/remove_cv.hh"
#include "cc/bzd/type_traits/remove_reference.hh"

namespace bzd::typeTraits {

template <class T>
using RemoveCVRef = RemoveCV<RemoveReference<T>>;

} // namespace bzd::typeTraits
