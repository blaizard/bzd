#pragma once

#include "bzd/types.h"
#include "bzd/meta/choose_nth.h"
#include "bzd/meta/contains.h"
#include "bzd/meta/find.h"

namespace bzd
{
	namespace meta
	{
		template <class... Ts>
		struct TypeList
		{
			using type = TypeList;
			static constexpr const SizeType size = sizeof...(Ts);

			template <SizeType N>
			using ChooseNth = typename impl::ChooseNth<N, Ts...>::type;

			template <class T>
			using Contains = typename impl::Contains<T, Ts...>;

			template <class T>
			using Find = typename impl::Find<0, T, Ts...>;
		};
	}
}
