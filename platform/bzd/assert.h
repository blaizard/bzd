#pragma once

#include "bzd/core/assert.h"
#include "bzd/core/system.h"
#include "bzd/log.h"
#include "bzd/container/expected.h"

namespace bzd { namespace assert {
template <class... Args>
constexpr void isTrue(const bool condition, Args&&... args)
{
	if (!condition)
	{
		bzd::log::print(bzd::forward<Args>(args)...);
		bzd::panic();
	}
}

template <class T, class E>
constexpr void isExpected(const Expected<T, E>& expected)
{
	if (!expected)
	{
		bzd::assert::isTrue(false, CSTR("Expected failed: {}"), expected.error());
	}
}

}} // namespace bzd::assert
