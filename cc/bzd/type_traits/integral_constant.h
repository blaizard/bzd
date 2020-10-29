#pragma once

namespace bzd::typeTraits::impl {
template <class T, T v>
struct IntegralConstant
{
	static constexpr T value = v;
	typedef T ValueType;
	typedef IntegralConstant type;
	constexpr operator ValueType() const noexcept { return value; }
	constexpr ValueType operator()() const noexcept { return value; }
};
} // namespace bzd::typeTraits::impl

namespace bzd::typeTraits {
template <class T, T v>
using IntegralConstant = impl::IntegralConstant<T, v>;

} // namespace bzd::typeTraits
