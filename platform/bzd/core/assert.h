#pragma once

#include "bzd/container/string_view.h"
#include "bzd/core/system.h"

namespace bzd { namespace assert {
constexpr void isTrue(const bool condition)
{
	if (!condition)
	{
		bzd::getOut().write(bzd::StringView{"Assertion failed.\n"});
		bzd::panic();
	}
}

constexpr void isTrue(const bool condition, const bzd::StringView& message)
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
