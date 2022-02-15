#pragma once

#include "cc/bzd/type_traits/is_default_constructible.hh"
#include "cc/bzd/type_traits/is_same.hh"
#include "cc/bzd/utility/forward.hh"
#include "cc/bzd/type_traits/derived_from.hh"

namespace bzd::test {
struct InjectPointBase
{
};

struct InjectPointNoMatch : InjectPointBase
{
};
}

namespace bzd::concepts
{
template <class T>
concept injectionPoint = derivedFrom<T, test::InjectPointBase>;
}

namespace bzd::test {
template <concepts::injectionPoint Id, concepts::injectionPoint MatchId = InjectPointNoMatch, class... Callables>
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

struct InjectPoint0 : InjectPointBase
{
};

struct InjectPoint1 : InjectPointBase
{
};

struct InjectPoint2 : InjectPointBase
{
};

struct InjectPoint3 : InjectPointBase
{
};

struct InjectPoint4 : InjectPointBase
{
};

struct InjectPoint5 : InjectPointBase
{
};

struct InjectPoint6 : InjectPointBase
{
};

struct InjectPoint7 : InjectPointBase
{
};

struct InjectPoint8 : InjectPointBase
{
};

struct InjectPoint9 : InjectPointBase
{
};

} // namespace bzd::test
