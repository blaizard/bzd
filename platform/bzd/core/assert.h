#pragma once

#include "bzd/container/string_view.h"
#include "bzd/core/system.h"

#include <assert.h>

namespace bzd { namespace assert {

namespace impl {
inline void assertHelper(bool test)
{
	assert(test);
}
} // namespace impl

constexpr bool isTrueConstexpr(const bool condition)
{
	return condition ? true : (impl::assertHelper(condition), false);
}

inline void isTrue(const bool condition)
{
	if (!condition)
	{
		bzd::getOut().write(bzd::StringView{"Assertion failed.\n"});
		bzd::panic();
	}
}

inline void isTrue(const bool condition, const bzd::StringView& message)
{
	if (!condition)
	{
		bzd::getOut().write(bzd::StringView{"Assertion failed with message: "});
		bzd::getOut().write(message);
		bzd::panic();
	}
}

void unreachable();

}} // namespace bzd::assert
