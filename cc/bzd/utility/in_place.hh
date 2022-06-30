#pragma once

#include "cc/bzd/platform/types.hh"

namespace bzd {
template <Size I>
struct InPlaceIndex
{
};
template <Size I>
constexpr InPlaceIndex<I> inPlaceIndex{};

template <class T>
struct InPlaceType
{
};
template <class T>
constexpr InPlaceType<T> inPlaceType{};

struct InPlace
{
};
constexpr InPlace inPlace{};
} // namespace bzd
