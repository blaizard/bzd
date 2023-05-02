#pragma once

namespace bzd::typeTraits::impl {
template <bool Condition, class T, class F>
struct Conditional
{
	typedef T Type;
};

template <class T, class F>
struct Conditional<false, T, F>
{
	typedef F Type;
};
} // namespace bzd::typeTraits::impl

namespace bzd::typeTraits {
template <bool Condition, class T, class F>
using Conditional = typename impl::Conditional<Condition, T, F>::Type;
} // namespace bzd::typeTraits
