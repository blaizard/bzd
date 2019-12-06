#pragma once

#include "bzd/type_traits/is_array.h"
#include "bzd/type_traits/is_function.h"
#include "bzd/type_traits/add_pointer.h"
#include "bzd/type_traits/remove_extent.h"
#include "bzd/type_traits/remove_reference.h"
#include "bzd/type_traits/remove_cv.h"
#include "bzd/type_traits/conditional.h"

namespace bzd
{
	template <class T>
	struct decay
	{
	private:
		typedef typename bzd::typeTraits::removeReference<T>::type U;
	public:
		typedef typename bzd::typeTraits::conditional<bzd::typeTraits::isArray<U>::value, typename bzd::typeTraits::removeExtent<U>::type*,
			typename bzd::typeTraits::conditional<bzd::typeTraits::isFunction<U>::value, typename bzd::typeTraits::addPointer<U>::type,
			typename bzd::typeTraits::removeCV<U>::type
        	>::type
    	>::type type;
	};
}
