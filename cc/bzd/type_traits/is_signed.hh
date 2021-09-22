#pragma once

#include "cc/bzd/type_traits/false_type.hh"
#include "cc/bzd/type_traits/integral_constant.hh"
#include "cc/bzd/type_traits/is_arithmetic.hh"

namespace bzd::typeTraits::impl {

template <class T, bool = bzd::typeTraits::isArithmetic<T>>
struct IsSigned : bzd::typeTraits::IntegralConstant<bool, T(-1) < T(0)>
{
};

template <class T>
struct IsSigned<T, false> : FalseType
{
};

} // namespace bzd::typeTraits::impl

namespace bzd::typeTraits {
template <class T>
using IsSigned = typename impl::IsSigned<T>;

template <class T>
constexpr bool isSigned = IsSigned<T>::value;

} // namespace bzd::typeTraits
