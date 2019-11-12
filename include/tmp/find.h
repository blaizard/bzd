#pragma once

#include "include/types.h"
#include "include/type_traits/is_same.h"

namespace bzd
{
	namespace tmp
	{
		namespace impl
		{
			template <SizeType N, class T, class U, class... Ts>
			struct Find
			{
				static constexpr const int value = (Find<N, T, U>::value >= 0) ? Find<N, T, U>::value : Find<N + 1, T, Ts...>::value;
			};

			template <SizeType N, class T, class U>
			struct Find<N, T, U>
			{
				static constexpr const int value = (bzd::typeTraits::isSame<T, U>::value) ? static_cast<int>(N) : -1;
			};
		}

		template <class T, class... Ts>
		using Find = typename impl::Find<0, T, Ts...>;
	}
}
