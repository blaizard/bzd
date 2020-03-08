#pragma once

#include "bzd/container/string_view.h"
#include "bzd/core/system.h"

namespace bzd { namespace assert {
namespace impl {
inline void assertHelper(bool test)
{
	throw "Assert error";
}

void backend(const bzd::StringView& message1, const bzd::StringView& message2 = {});

} // namespace impl

constexpr bool isTrueConstexpr(const bool condition)
{
	return (condition) ? true : (impl::assertHelper(condition), false);
}

constexpr void isTrue(const bool condition)
{
	if (!condition)
	{
		impl::backend("Assertion failed.\n");
	}
}

constexpr void isTrue(const bool condition, const bzd::StringView& message)
{
	if (!condition)
	{
		impl::backend("Assertion failed with message: ", message);
	}
}

void unreachable();

}} // namespace bzd::assert
