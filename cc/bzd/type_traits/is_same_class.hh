#pragma once

#include "cc/bzd/type_traits/is_same.hh"
#include "cc/bzd/type_traits/remove_cvref.hh"

namespace bzd::concepts {
template <class T, class U>
concept sameClassAs = sameAs<typeTraits::RemoveCVRef<T>, typeTraits::RemoveCVRef<U>>;
}
