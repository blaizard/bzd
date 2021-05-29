#pragma once

#include "cc/bzd/utility/source_location.h"

namespace bzd::assert::impl {
inline void assertHelper(bool /*test*/)
{
	throw "Assert error";
}

void backend(const bzd::SourceLocation& location, const char* message1, const char* message2 = nullptr);

} // namespace bzd::assert::impl

namespace bzd::assert {

constexpr bool isTrueConstexpr(const bool condition)
{
	return (condition) ? true : (impl::assertHelper(condition), false);
}

constexpr void isTrue(const bool condition, const bzd::SourceLocation location = bzd::SourceLocation::current())
{
	if (!condition)
	{
		impl::backend(location, "Assertion failed.\n");
	}
}

constexpr void isTrue(const bool condition, const char* message, const bzd::SourceLocation location = bzd::SourceLocation::current())
{
	if (!condition)
	{
		impl::backend(location, "Assertion failed with message: ", message);
	}
}

void unreachable(const bzd::SourceLocation location = bzd::SourceLocation::current());

} // namespace bzd::assert
