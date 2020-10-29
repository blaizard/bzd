#pragma once

#include "bzd/type_traits/remove_reference.h"

namespace bzd::typeTraits::impl {
template <class T>
struct TypeIdentity
{
	using type = T;
};
template <class T>
auto tryAddPointer(int) -> TypeIdentity<bzd::typeTraits::RemoveReference<T>*>;
template <class T>
auto tryAddPointer(...) -> TypeIdentity<T>;
template <class T>
struct AddPointer : decltype(impl::tryAddPointer<T>(0))
{
};
} // namespace bzd::typeTraits::impl

namespace bzd::typeTraits {
template <class T>
using AddPointer = typename impl::AddPointer<T>::type;
} // namespace bzd::typeTraits
