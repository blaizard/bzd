#pragma once

#include "cc/bzd/container/reference_wrapper.hh"
#include "cc/bzd/container/value_wrapper.hh"
#include "cc/bzd/type_traits/conditional.hh"
#include "cc/bzd/type_traits/is_lvalue_reference.hh"

namespace bzd {

template <class T>
using Wrapper = bzd::typeTraits::Conditional<typeTraits::isLValueReference<T>,
											 bzd::ReferenceWrapper<typeTraits::RemoveReference<T>>,
											 bzd::ValueWrapper<typeTraits::RemoveReference<T>>>;

}
