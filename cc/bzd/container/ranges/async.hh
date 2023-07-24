#pragma once

#include "cc/bzd/core/async.hh"
#include "cc/bzd/type_traits/declval.hh"
#include "cc/bzd/type_traits/range.hh"
#include "cc/bzd/type_traits/remove_cvref.hh"

/// Not sure why the move constructor for Generator end up into a segfault, to be investigated.
template <bzd::concepts::generatorInputByteCopyableRange Generator, class Adaptor>
auto operator|(Generator& generator, Adaptor&& adaptor) noexcept
	-> bzd::Generator<decltype(adaptor(bzd::typeTraits::declval<typename bzd::typeTraits::RemoveCVRef<Generator>::ResultType::Value&>()))>
{
	while (!generator.isCompleted())
	{
		auto maybeRange = co_await generator;
		if (!maybeRange)
		{
			co_yield bzd::move(maybeRange).propagate();
		}
		else
		{
			co_yield adaptor(maybeRange.valueMutable());
		}
	}
}
