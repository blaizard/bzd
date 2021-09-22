#pragma once

#include "cc/bzd/container/result.hh"
#include "cc/bzd/core/assert/minimal.hh"
#include "cc/bzd/core/logger.hh"
#include "cc/bzd/platform/panic.hh"
#include "cc/bzd/utility/source_location.hh"

namespace bzd::assert {
template <class... Args>
constexpr void isTrue(const bool condition, Args&&... args)
{
	if (!condition)
	{
		bzd::log::error(bzd::forward<Args>(args)...);
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
