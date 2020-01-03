#pragma once

#include "bzd/container/constexpr_string_view.h"
#include "bzd/core/assert.h"
#include "bzd/core/system.h"
#include "bzd/log.h"

namespace bzd { namespace assert {
template <class... Args>
constexpr void isTrue(const bool condition, Args &&... args)
{
	if (!condition)
	{
		bzd::log::print(bzd::forward<Args>(args)...);
		bzd::panic();
	}
}
}}	// namespace bzd::assert
