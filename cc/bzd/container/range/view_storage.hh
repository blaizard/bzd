#pragma once

#include "cc/bzd/container/reference_wrapper.hh"
#include "cc/bzd/container/value_wrapper.hh"
#include "cc/bzd/type_traits/conditional.hh"

namespace bzd::range {

template <class Range>
using ViewStorage = bzd::typeTraits::Conditional<typeTraits::isLValueReference<Range>,
												 bzd::ReferenceWrapper<typeTraits::RemoveReference<Range>>,
												 bzd::ValueWrapper<typeTraits::RemoveReference<Range>>>;

}
