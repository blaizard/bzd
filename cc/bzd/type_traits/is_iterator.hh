#pragma once

#include "cc/bzd/container/iterator/traits.hh"
#include "cc/bzd/type_traits/false_type.hh"
#include "cc/bzd/type_traits/true_type.hh"
#include "cc/bzd/type_traits/void_type.hh"
#include "cc/bzd/utility/concept.hh"

namespace bzd::typeTraits::impl {

template <class T, class = void>
struct IsIterator : bzd::typeTraits::FalseType
{
};

template <class T>
struct IsIterator<T,
				  bzd::typeTraits::VoidType<typename bzd::iterator::Traits<T>::Category,
											typename bzd::iterator::Traits<T>::ValueType,
											typename bzd::iterator::Traits<T>::DifferenceType>> : bzd::typeTraits::TrueType
{
};

} // namespace bzd::typeTraits::impl

namespace bzd::typeTraits {

template <class T>
CONCEPT isIterator = impl::IsIterator<T>::value;

}
