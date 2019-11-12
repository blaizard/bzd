#pragma once

#include "include/types.h"
#include "include/tmp/choose_nth.h"
#include "include/tmp/contains.h"
#include "include/tmp/find.h"

namespace bzd
{
	namespace tmp
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
