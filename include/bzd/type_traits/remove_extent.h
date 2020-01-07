#pragma once

namespace bzd { namespace typeTraits {
template <class T>
struct removeExtent
{
	typedef T type;
};
template <class T>
struct removeExtent<T[]>
{
	typedef T type;
};
template <class T, unsigned long int N>
struct removeExtent<T[N]>
{
	typedef T type;
};
}} // namespace bzd::typeTraits
