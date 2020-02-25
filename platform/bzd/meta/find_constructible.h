#pragma once

#include "bzd/type_traits/is_constructible.h"
#include "bzd/types.h"

namespace bzd { namespace meta {
namespace impl {
template <SizeType N, class T, class U, class... Ts>
struct FindConstructible
{
	static constexpr const int value = (FindConstructible<N, T, U>::value >= 0) ? FindConstructible<N, T, U>::value : FindConstructible<N + 1, T, Ts...>::value;
};

template <SizeType N, class T, class U>
struct FindConstructible<N, T, U>
{
	static constexpr const int value = (bzd::typeTraits::isConstructible<U, T>) ? static_cast<int>(N) : -1;
};
} // namespace impl

template <class T, class... Ts>
using FindConstructible = typename impl::FindConstructible<0, T, Ts...>;
}} // namespace bzd::meta
