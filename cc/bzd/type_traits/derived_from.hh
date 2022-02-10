#pragma once

#include "cc/bzd/type_traits/is_base_of.hh"
#include "cc/bzd/type_traits/is_convertible.hh"

namespace bzd::concepts {

template <class Derived, class Base>
concept derivedFrom = typeTraits::isBaseOf<Base, Derived> && typeTraits::isConvertible<const volatile Derived*, const volatile Base*>;

}
