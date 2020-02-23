#pragma once

#include "bzd/type_traits/add_reference.h"

namespace bzd { namespace typeTraits { namespace impl {
template <class T>
struct AddLValueReference
{
	typedef AddReference<T> type;
};

template <class T>
struct AddLValueReference<T&&>
{
	typedef T& type;
};
}

template <class T>
using AddLValueReference = typename impl::AddLValueReference<T>::type;

}} // namespace bzd::typeTraits
