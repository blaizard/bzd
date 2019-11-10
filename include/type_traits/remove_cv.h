#pragma once

#include "include/type_traits/remove_const.h"
#include "include/type_traits/remove_volatile.h"

namespace bzd
{
	namespace typeTraits
	{
		template <class T>
		struct removeCV
		{
			typedef typename removeVolatile<typename removeConst<T>::type>::type type;
		};
	}
}
