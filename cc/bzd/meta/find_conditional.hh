#pragma once

#include "cc/bzd/platform/types.hh"

namespace bzd::meta::impl {
template <Size N, template <class, class> class Condition, class T, class U, class... Ts>
struct FindConditional
{
	static constexpr const int value = (FindConditional<N, Condition, T, U>::value >= 0)
										   ? FindConditional<N, Condition, T, U>::value
										   : FindConditional<N + 1, Condition, T, Ts...>::value;
};

template <Size N, template <class, class> class Condition, class T, class U>
struct FindConditional<N, Condition, T, U>
{
	static constexpr const int value = (Condition<T, U>::value) ? static_cast<int>(N) : -1;
};
} // namespace bzd::meta::impl

namespace bzd::meta {
template <template <class, class> class Condition, class T, class... Ts>
using FindConditional = typename impl::FindConditional<0, Condition, T, Ts...>;
} // namespace bzd::meta
