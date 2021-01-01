#pragma once

#include "bzd/type_traits/is_default_constructible.h"
#include "bzd/type_traits/is_same.h"
namespace bzd::test {
template <class Id, class MatchId = void, class... Callables>
class InjectPoint
{
private: // Preconditions
	static_assert(!typeTraits::isSame<Id, void>, "Invalid identifier type.");
	static_assert((typeTraits::isDefaultConstructible<Callables> && ...),
				  "Only default constructible types should be passed as arguments.");

public:
	constexpr InjectPoint() : InjectPoint([]() {}) {}

	template <class T>
	constexpr InjectPoint(const T& callable)
	{
		if constexpr (typeTraits::isSame<Id, MatchId>)
		{
			callable();
			(Callables(), ...);
		}
	}
};

} // namespace bzd::test
