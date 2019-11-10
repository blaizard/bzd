#pragma once

#include "include/type_traits/is_reference.h"
#include "include/type_traits/is_void.h"

namespace bzd
{
	namespace typeTraits
	{
		namespace impl
		{
			template <typename T, bool b>
			struct addRValueReferenceHelper { typedef T type; };

			template <typename T>
			struct addRValueReferenceHelper<T, true> { typedef T&& type; };

			template <typename T>
			struct addRValueReference
			{
				typedef typename addRValueReferenceHelper<T, (isVoid<T>::value == false && isReference<T>::value == false) >::type type;
			};
		}

		template <class T>
		struct addRValueReference { typedef typename impl::addRValueReference<T>::type type; };
	}
}
