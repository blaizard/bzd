#pragma once

#include "bzd/types.h"

namespace bzd { namespace meta {

namespace range {
template <SizeType... N>
struct Type
{
	typedef Type<N...> type;
};
} // namespace range

namespace impl {

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

} // namespace impl

template <SizeType Start, SizeType End>
using Range = typename impl::Range<Start, End>::type;

}} // namespace bzd::meta
