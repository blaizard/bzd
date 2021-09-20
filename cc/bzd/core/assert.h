#pragma once

#include "cc/bzd/container/result.h"
#include "cc/bzd/core/assert/minimal.h"
#include "cc/bzd/core/logger/minimal.h"
#include "cc/bzd/platform/panic.h"
#include "cc/bzd/utility/source_location.h"

namespace bzd::assert {
template <class... Args>
constexpr void isTrue(const bool condition, Args&&... args)
{
	if (!condition)
	{
		bzd::minimal::Logger::getDefault().info(bzd::forward<Args>(args)...);
		bzd::platform::panic();
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

} // namespace bzd::assert
