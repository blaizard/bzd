#pragma once

#include "bzd/container/result.h"
#include "bzd/core/assert/minimal.h"
#include "bzd/core/log.h"
#include "bzd/platform/panic.h"
#include "bzd/utility/source_location.h"

namespace bzd::assert {
template <class... Args>
constexpr void isTrue(const bool condition, Args&&... args)
{
	if (!condition)
	{
		bzd::log::print(bzd::forward<Args>(args)...);
		bzd::platform::panic();
	}
}

template <class T, class E>
constexpr void isResult(const Result<T, E>& result)
{
	if (!result)
	{
		bzd::assert::isTrue(false, "Result failed: {}"_sv, result.error());
	}
}

} // namespace bzd::assert
