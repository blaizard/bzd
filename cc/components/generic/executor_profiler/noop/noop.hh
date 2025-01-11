#pragma once

#include "cc/bzd/platform/types.hh"

namespace bzd::components::generic {

class ExecutorProfilerNoop
{
public:
	template <class Event>
	constexpr void event(const Event) const noexcept
	{
	}
};

ExecutorProfilerNoop& getExecutorProfilerNoop() noexcept;

} // namespace bzd::components::generic
