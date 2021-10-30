#pragma once

#include "cc/bzd/platform/types.hh"
#include "cc/bzd/type_traits/decay.hh"
#include "cc/bzd/type_traits/integral_constant.hh"

namespace bzd {

template <Int32Type Start, Int32Type End, Int32Type Inc, class F>
constexpr void constexprFor(F&& f) noexcept
{
	static_assert(Inc != 0, "The increment cannot be equal to 0.");

	if constexpr ((Inc > 0 && Start < End) || (Inc < 0 && Start > End))
	{
		f(typeTraits::IntegralConstant<decltype(Start), Start>());
		constexprFor<Start + Inc, End, Inc>(f);
	}
}

template <class Container, class F>
constexpr void constexprForContainerInc(Container&& container, F&& f) noexcept
{
	constexprFor<0, bzd::typeTraits::Decay<Container>::size(), 1>([&](auto i) { f(container.template get<i.value>()); });
}

template <class Container, class F>
constexpr void constexprForContainerDec(Container&& container, F&& f) noexcept
{
	constexprFor<static_cast<Int32Type>(typeTraits::Decay<Container>::size()) - 1, -1, -1>(
		[&](auto i) { f(container.template get<i.value>()); });
}

} // namespace bzd
