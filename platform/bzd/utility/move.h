#pragma once

#include "bzd/type_traits/remove_reference.h"

namespace bzd {
template <class T>
typename bzd::typeTraits::removeReference<T>::type&& move(T&& arg)
{
	return static_cast<typename bzd::typeTraits::removeReference<T>::type&&>(arg);
}
} // namespace bzd
