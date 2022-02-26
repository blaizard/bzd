#pragma once

#include "cc/bzd/type_traits/false_type.hh"
#include "cc/bzd/type_traits/remove_cvref.hh"
#include "cc/bzd/type_traits/true_type.hh"

namespace bzd::typeTraits::impl {
template <class, template <class...> class>
struct IsSameTemplate : FalseType
{
};

template <template <class...> class T, class... Args>
struct IsSameTemplate<T<Args...>, T> : TrueType
{
};
} // namespace bzd::typeTraits::impl

namespace bzd::typeTraits {
template <class T, template <class...> class U>
using IsSameTemplate = typename impl::IsSameTemplate<RemoveCVRef<T>, U>;

template <class T, template <class...> class U>
inline constexpr bool isSameTemplate = IsSameTemplate<T, U>::value;

} // namespace bzd::typeTraits

namespace bzd::concepts {
template <class T, template <class...> class U>
concept sameTemplate = typeTraits::isSameTemplate<T, U>;
}
