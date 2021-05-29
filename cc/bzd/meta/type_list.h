#pragma once

#include "cc/bzd/meta/choose_nth.h"
#include "cc/bzd/meta/contains.h"
#include "cc/bzd/meta/find.h"
#include "cc/bzd/meta/find_conditional.h"
#include "cc/bzd/platform/types.h"

namespace bzd::meta {
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
} // namespace bzd::meta
