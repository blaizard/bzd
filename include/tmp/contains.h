#pragma once

#include "include/type_traits/is_same.h"

namespace bzd
{
	namespace tmp
	{
		namespace impl
		{
			template <class T, class U, class... Ts>
			struct Contains
			{
				static constexpr const bool value = Contains<T, U>::value || Contains<T, Ts...>::value;
			};

			template <class T, class U>
			struct Contains<T, U>
			{
				static constexpr const bool value = bzd::typeTraits::isSame<T, U>::value;
			};
		}

		template <class T, class... Ts>
		using Contains = typename impl::Contains<T, Ts...>;
	}
}
