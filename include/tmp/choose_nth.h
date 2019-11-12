#pragma once

#include "include/types.h"
#include "include/tmp/type.h"

namespace bzd
{
	namespace tmp
	{
		namespace impl
		{
			template <SizeType N, class... Ts>
			struct ChooseNth;

			template <SizeType N, class H, class... Ts>
			struct ChooseNth<N, H, Ts...> : ChooseNth<N - 1, Ts...> {};

			template <class H, class... Ts>
			struct ChooseNth<0, H, Ts...> : Type<H> {};
		}

		template <SizeType N, class... Ts>
		using ChooseNth = typename impl::ChooseNth<N, Ts...>::type;
	}
}
