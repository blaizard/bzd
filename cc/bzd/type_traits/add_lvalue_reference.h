#pragma once

#include "cc/bzd/type_traits/add_reference.h"

namespace bzd::typeTraits::impl {
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
} // namespace bzd::typeTraits::impl

namespace bzd::typeTraits {
template <class T>
using AddLValueReference = typename impl::AddLValueReference<T>::type;
} // namespace bzd::typeTraits
