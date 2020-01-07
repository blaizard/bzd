#pragma once

namespace bzd { namespace typeTraits {
template <class T>
struct removeReference
{
	typedef T type;
};

template <class T>
struct removeReference<T &>
{
	typedef T type;
};

template <class T>
struct removeReference<T &&>
{
	typedef T type;
};
}} // namespace bzd::typeTraits
