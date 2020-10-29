#pragma once

#include "bzd/container/result.h"
#include "bzd/core/assert.h"
#include "bzd/core/system.h"
#include "bzd/log.h"

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
constexpr void isResult(const Result<T, E>& result)
{
	if (!result)
	{
		bzd::assert::isTrue(false, CSTR("Result failed: {}"), result.error());
	}
}

}} // namespace bzd::assert
