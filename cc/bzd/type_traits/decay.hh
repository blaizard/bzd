#pragma once

#include "cc/bzd/type_traits/add_pointer.hh"
#include "cc/bzd/type_traits/conditional.hh"
#include "cc/bzd/type_traits/is_array.hh"
#include "cc/bzd/type_traits/is_function.hh"
#include "cc/bzd/type_traits/remove_cv.hh"
#include "cc/bzd/type_traits/remove_extent.hh"
#include "cc/bzd/type_traits/remove_reference.hh"

namespace bzd::typeTraits::impl {
template <class T>
struct Decay
{
private:
	typedef bzd::typeTraits::RemoveReference<T> U;

public:
	typedef bzd::typeTraits::Conditional<
		bzd::typeTraits::isArray<U>,
		bzd::typeTraits::RemoveExtent<U>*,
		bzd::typeTraits::Conditional<bzd::typeTraits::isFunction<U>, bzd::typeTraits::AddPointer<U>, bzd::typeTraits::RemoveCV<U>>>
		type;
};
} // namespace bzd::typeTraits::impl

namespace bzd::typeTraits {
template <class T>
using Decay = typename impl::Decay<T>::type;
} // namespace bzd::typeTraits
