#include "bzd/type_traits/integral_constant.h"
#include "bzd/type_traits/remove_cv.h"

namespace bzd::typeTraits::impl {
template <class T>
struct IsUnion : public bzd::typeTraits::IntegralConstant<bool, __is_union(RemoveCV<T>)>
{
};
} // namespace bzd::typeTraits::impl

namespace bzd::typeTraits {
template <class T>
using IsUnion = typename impl::IsUnion<T>;

template <class T>
constexpr bool isUnion = IsUnion<T>::value;
} // namespace bzd::typeTraits
