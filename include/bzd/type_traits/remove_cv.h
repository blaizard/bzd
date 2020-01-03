#pragma once

#include "bzd/type_traits/remove_const.h"
#include "bzd/type_traits/remove_volatile.h"

namespace bzd { namespace typeTraits {
template <class T>
struct removeCV
{
	typedef typename removeVolatile<typename removeConst<T>::type>::type type;
};
}}	// namespace bzd::typeTraits
