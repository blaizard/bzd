#pragma once

#include "include/type_traits/integral_constant.h"
#include "include/type_traits/void_type.h"
#include "include/type_traits/declval.h"

namespace bzd
{
	namespace typeTraits
	{
		namespace impl
		{
			template <class, class T, class... Args>
			struct isConstructible : public falseType {};

			template <class T, class... Args>
			struct isConstructible<voidType<decltype(T(declval<Args>()...))>, T, Args...> : public trueType {};
		}

		template <class T, class... Args>
		using isConstructible = impl::isConstructible<voidType<>, T, Args...>;
	}
}
