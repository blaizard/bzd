#pragma once

namespace bzd { namespace typeTraits {
namespace impl {
template <class T>
struct AddCV
{
	typedef const volatile T type;
};
} // namespace impl

template <class T>
using AddCV = typename impl::AddCV<T>::type;
}} // namespace bzd::typeTraits
