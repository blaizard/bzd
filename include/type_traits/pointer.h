#pragma once

#include "include/type_traits/reference.h"

namespace bzd
{
	namespace typeTraits
	{
		namespace impl
		{
			template <class T> struct TypeIdentity { using type = T; };
 			template <class T> auto tryAddPointer(int) -> TypeIdentity<typename bzd::typeTraits::removeReference<T>::type*>;
			template <class T> auto tryAddPointer(...) -> TypeIdentity<T>;
 		}

		// addPointer

		template <class T> struct addPointer : decltype(impl::tryAddPointer<T>(0)) {};
	}
}
