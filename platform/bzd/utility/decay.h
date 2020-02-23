#pragma once

#include "bzd/type_traits/add_pointer.h"
#include "bzd/type_traits/conditional.h"
#include "bzd/type_traits/is_array.h"
#include "bzd/type_traits/is_function.h"
#include "bzd/type_traits/remove_cv.h"
#include "bzd/type_traits/remove_extent.h"
#include "bzd/type_traits/remove_reference.h"

namespace bzd {
template <class T>
struct decay
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
} // namespace bzd
