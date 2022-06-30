#pragma once

#include "cc/bzd/meta/choose_nth.hh"
#include "cc/bzd/meta/contains.hh"
#include "cc/bzd/meta/find.hh"
#include "cc/bzd/meta/find_conditional.hh"
#include "cc/bzd/platform/types.hh"

namespace bzd::meta {
template <class... Ts>
struct TypeList
{
	using type = TypeList;
	static constexpr const Size size = sizeof...(Ts);

	template <Size N>
	using ChooseNth = ChooseNth<N, Ts...>;

	template <class T>
	using Contains = Contains<T, Ts...>;

	template <class T>
	using Find = Find<T, Ts...>;

	template <class T, template <class, class> class Condition>
	using FindConditional = FindConditional<Condition, T, Ts...>;
};
} // namespace bzd::meta
