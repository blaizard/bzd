#pragma once

#include "bzd/type_traits/add_reference.h"

namespace bzd { namespace typeTraits {
template <class T>
struct addLValueReference
{
	typedef typename addReference<T>::type type;
};

template <class T>
struct addLValueReference<T &&>
{
	typedef T &type;
};
}} // namespace bzd::typeTraits
