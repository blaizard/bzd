#pragma once

#include "cc/bzd/platform/types.hh"

namespace bzd::meta::ranges {
template <Size... N>
struct Type
{
	typedef Type<N...> type;
};
} // namespace bzd::meta::ranges

namespace bzd::meta::impl {

template <Size C, Size P, Size... N>
struct GetRange : GetRange<C - 1, P + 1, N..., P>
{
};

template <Size P, Size... N>
struct GetRange<0, P, N...> : bzd::meta::ranges::Type<N...>
{
};

template <Size Start, Size End>
struct Range : GetRange<End - Start, Start>
{
};

} // namespace bzd::meta::impl

namespace bzd::meta {

/// Create a linear integer sequence from [Start, End[ (including start and excluding end).
///
/// The type can then be deduced with `bzd::meta::ranges::Type<N...>`.
template <Size Start, Size End>
using Range = typename impl::Range<Start, End>::type;

} // namespace bzd::meta
