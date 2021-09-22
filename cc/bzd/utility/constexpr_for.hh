#pragma once

#include "cc/bzd/type_traits/decay.hh"
#include "cc/bzd/type_traits/integral_constant.hh"

namespace bzd {

template <int Start, int End, int Inc, class F>
constexpr void constexprFor(F&& f) noexcept
{
	static_assert(Inc != 0, "The increment cannot be equal to 0.");

	if constexpr ((Inc > 0 && Start < End) || (Inc < 0 && Start > End))
	{
		f(bzd::typeTraits::IntegralConstant<decltype(Start), Start>());
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
	constexprFor<bzd::typeTraits::Decay<Container>::size() - 1, -1, -1>([&](auto i) { f(container.template get<i.value>()); });
}

} // namespace bzd
