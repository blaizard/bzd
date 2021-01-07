#pragma once

#include "bzd/type_traits/is_default_constructible.h"
#include "bzd/type_traits/is_same.h"
#include "bzd/utility/forward.h"

namespace bzd::test {
template <class Id, class MatchId = void, class... Callables>
class InjectPoint
{
private: // Preconditions
	static_assert(!typeTraits::isSame<Id, void>, "Invalid identifier type.");
	//	static_assert((typeTraits::isDefaultConstructible<Callables> && ...),
	//				  "Only default constructible types should be passed as arguments.");

public:
	template <class... Args>
	constexpr InjectPoint(Args&&... args)
	{
		if constexpr (typeTraits::isSame<Id, MatchId>)
		{
			(Callables(bzd::forward<Args>(args)...), ...);
		}
	}
};

} // namespace bzd::test
