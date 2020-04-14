#pragma once

#include "bzd/meta/choose_nth.h"
#include "bzd/meta/contains.h"
#include "bzd/meta/find.h"
#include "bzd/meta/find_conditional.h"
#include "bzd/types.h"

namespace bzd { namespace meta {
template <class... Ts>
struct TypeList
{
	using type = TypeList;
	static constexpr const SizeType size = sizeof...(Ts);

	template <SizeType N>
	using ChooseNth = ChooseNth<N, Ts...>;

	template <class T>
	using Contains = Contains<T, Ts...>;

	template <class T>
	using Find = Find<T, Ts...>;

	template <class T, template <class, class> class Condition>
	using FindConditional = FindConditional<Condition, T, Ts...>;
};
}} // namespace bzd::meta
