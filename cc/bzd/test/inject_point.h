#pragma once

#include "cc/bzd/type_traits/is_same.h"
#include "cc/bzd/type_traits/is_function.h"

namespace bzd::test {

template <class Id, class MatchId = void, class... Callables>
class InjectPoint
{
private: // Preconditions
	static_assert(!typeTraits::isSame<Id, void>, "Invalid identifier type.");
	//static_assert((typeTraits::isFunction<Callables> && ...), "Only callables should be passed as arguments.");

public:
	constexpr InjectPoint()
	{
		if constexpr (typeTraits::isSame<Id, MatchId>) {
			(Callables(), ...);
		}
	}
};

}
