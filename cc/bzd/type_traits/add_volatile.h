#pragma once

namespace bzd { namespace typeTraits {
namespace impl {
template <class T>
struct AddVolatile
{
	typedef volatile T type;
};
} // namespace impl

template <class T>
using AddVolatile = typename impl::AddVolatile<T>::type;

}} // namespace bzd::typeTraits
