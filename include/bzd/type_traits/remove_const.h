#pragma once

namespace bzd { namespace typeTraits {
template <class T>
struct removeConst
{
	typedef T type;
};
template <class T>
struct removeConst<const T>
{
	typedef T type;
};
}} // namespace bzd::typeTraits
