#pragma once

#include "cc/bzd/platform/types.hh"

namespace bzd {
template <SizeType I>
struct InPlaceIndex
{
};
template <SizeType I>
constexpr InPlaceIndex<I> inPlaceIndex{};

template <class T>
struct InPlaceType
{
};
template <class T>
constexpr InPlaceType<T> inPlaceType{};
} // namespace bzd
