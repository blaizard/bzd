#pragma once

#include "bzd/type_traits/remove_reference.h"

namespace bzd { namespace typeTraits {
namespace impl {
template <class T>
struct TypeIdentity
{
	using type = T;
};
template <class T>
auto tryAddPointer(int) -> TypeIdentity<typename bzd::typeTraits::removeReference<T>::type*>;
template <class T>
auto tryAddPointer(...) -> TypeIdentity<T>;
template <class T>
struct AddPointer : decltype(impl::tryAddPointer<T>(0))
{
};
} // namespace impl

template <class T>
using AddPointer = typename impl::AddPointer<T>::type;

}} // namespace bzd::typeTraits
