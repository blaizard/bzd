#pragma once

#include "cc/bzd/platform/types.hh"

namespace bzd::meta::range {
template <SizeType... N>
struct Type
{
	typedef Type<N...> type;
};
} // namespace bzd::meta::range

namespace bzd::meta::impl {

template <SizeType C, SizeType P, SizeType... N>
struct GetRange : GetRange<C - 1, P + 1, N..., P>
{
};

template <SizeType P, SizeType... N>
struct GetRange<0, P, N...> : bzd::meta::range::Type<N...>
{
};

template <SizeType Start, SizeType End>
struct Range : GetRange<End - Start, Start>
{
};

} // namespace bzd::meta::impl

namespace bzd::meta {

/// Create a linear integer sequence from [Start, End[ (including start and excluding end).
///
/// The type can then be deduced with `bzd::meta::range::Type<N...>`.
template <SizeType Start, SizeType End>
using Range = typename impl::Range<Start, End>::type;

} // namespace bzd::meta
