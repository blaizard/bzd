#pragma once

#include "bzd/meta/type.h"
#include "bzd/types.h"

namespace bzd { namespace meta {
namespace impl {
template <SizeType N, class... Ts>
struct ChooseNth;

template <SizeType N, class H, class... Ts>
struct ChooseNth<N, H, Ts...> : ChooseNth<N - 1, Ts...>
{
};

template <class H, class... Ts>
struct ChooseNth<0, H, Ts...> : Type<H>
{
};
}  // namespace impl

template <SizeType N, class... Ts>
using ChooseNth = typename impl::ChooseNth<N, Ts...>::type;
}}	// namespace bzd::meta
