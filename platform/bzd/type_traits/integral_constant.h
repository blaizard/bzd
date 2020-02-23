#pragma once

namespace bzd { namespace typeTraits {
template <class T, T v>
struct integralConstant
{
	static constexpr T value = v;
	typedef T valueType;
	typedef integralConstant type;
	constexpr operator valueType() const noexcept { return value; }
	constexpr valueType operator()() const noexcept { return value; }
};

template <class T, T v>
using IntegralConstant = typename integralConstant<T, v>::type;

}} // namespace bzd::typeTraits
