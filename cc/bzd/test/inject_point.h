#pragma once

#include "cc/bzd/type_traits/is_default_constructible.h"
#include "cc/bzd/type_traits/is_same.h"
#include "cc/bzd/utility/forward.h"

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

struct InjectPoint0
{
};

struct InjectPoint1
{
};

struct InjectPoint2
{
};

struct InjectPoint3
{
};

struct InjectPoint4
{
};

struct InjectPoint5
{
};

struct InjectPoint6
{
};

struct InjectPoint7
{
};

struct InjectPoint8
{
};

struct InjectPoint9
{
};

} // namespace bzd::test
