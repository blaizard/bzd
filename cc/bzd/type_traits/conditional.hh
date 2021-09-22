#pragma once

namespace bzd::typeTraits::impl {
template <bool Condition, class T, class F>
struct Conditional
{
	typedef T type;
};

template <class T, class F>
struct Conditional<false, T, F>
{
	typedef F type;
};
} // namespace bzd::typeTraits::impl

namespace bzd::typeTraits {
template <bool Condition, class T, class F>
using Conditional = typename impl::Conditional<Condition, T, F>::type;
} // namespace bzd::typeTraits
