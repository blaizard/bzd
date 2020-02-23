#pragma once

namespace bzd { namespace typeTraits { namespace impl {
template <class T>
struct AddConst
{
	typedef const T type;
};
}

template <class T>
using AddConst = typename impl::AddConst<T>::type;

}} // namespace bzd::typeTraits
