#pragma once

#include "bzd/type_traits/true_type.h"
#include "bzd/type_traits/false_type.h"
#include "bzd/type_traits/remove_cv.h"

namespace bzd { namespace typeTraits {
namespace impl {

template <typename T>
struct IsPointer : public FalseType
{
};

template <typename T>
struct IsPointer<T*> : public TrueType
{
};

}

template <class T>
using IsPointer = typename impl::IsPointer<RemoveCV<T>>;

template <class T>
constexpr bool isPointer = IsPointer<T>::value;

}}
