#pragma once

#include "cc/bzd/platform/types.hh"
#include "cc/bzd/type_traits/decay.hh"
#include "cc/bzd/type_traits/integral_constant.hh"
#include "cc/bzd/type_traits/invoke_result.hh"
#include "cc/bzd/type_traits/is_same.hh"

namespace bzd {

template <Int32 Start, Int32 End, Int32 Inc, class F>
constexpr Int32 constexprFor(F&& f) noexcept
{
	static_assert(Inc != 0, "The increment cannot be equal to 0.");

	if constexpr ((Inc > 0 && Start < End) || (Inc < 0 && Start > End))
	{
		constexpr auto argument = typeTraits::IntegralConstant<decltype(Start), Start>();
		constexpr Bool fReturns = !typeTraits::isSame<typeTraits::InvokeResult<F, decltype(argument)>, void>;

		if constexpr (fReturns)
		{
			if (f(argument))
			{
				return constexprFor<Start + Inc, End, Inc>(f);
			}
		}
		else
		{
			f(argument);
			return constexprFor<Start + Inc, End, Inc>(f);
		}
	}

	return Start;
}

template <class Container, class F>
constexpr Int32 constexprForContainerInc(Container&& container, F&& f) noexcept
{
	return constexprFor<0, bzd::typeTraits::Decay<Container>::size(), 1>([&](auto i) { return f(container.template get<i.value>()); });
}

template <class Container, class F>
constexpr Int32 constexprForContainerDec(Container&& container, F&& f) noexcept
{
	return constexprFor<static_cast<Int32>(typeTraits::Decay<Container>::size()) - 1, -1, -1>(
		[&](auto i) { return f(container.template get<i.value>()); });
}

} // namespace bzd
