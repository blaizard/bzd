#pragma once

#include "cc/bzd/platform/types.hh"

namespace bzd::components::generic {

class CoreProfilerNoop
{
public:
	template <class Event>
	constexpr void event(const Event) const noexcept
	{
	}
};

CoreProfilerNoop& getCoreProfilerNoop() noexcept;

} // namespace bzd::components::generic
