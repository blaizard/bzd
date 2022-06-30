#pragma once

#include "cc/bzd/meta/type.hh"
#include "cc/bzd/platform/types.hh"

namespace bzd::meta::impl {
template <Size N, class... Ts>
struct ChooseNth;

template <Size N, class H, class... Ts>
struct ChooseNth<N, H, Ts...> : ChooseNth<N - 1, Ts...>
{
};

template <class H, class... Ts>
struct ChooseNth<0, H, Ts...> : Type<H>
{
};
} // namespace bzd::meta::impl
namespace bzd::meta {
template <Size N, class... Ts>
using ChooseNth = typename impl::ChooseNth<N, Ts...>::type;
} // namespace bzd::meta
