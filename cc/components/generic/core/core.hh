#pragma once

#include "cc/bzd/container/optional.hh"
#include "cc/bzd/container/result.hh"
#include "cc/bzd/container/stack.hh"
#include "cc/components/generic/core/interface.hh"

namespace bzd::platform::generic {

class Core : public bzd::platform::Core<Core>
{
private:
	using Self = Core;
	using Parent = bzd::platform::Core<Self>;

public:
	explicit Core() noexcept {}

	void init() {}

	bzd::Result<void, bzd::Error> stop() noexcept { return bzd::nullresult; }

	bzd::Result<void, bzd::Error> start(const bzd::platform::WorkloadType& workload) noexcept
	{
		workload();
		return bzd::nullresult;
	}

	StackSize getStackUsage() noexcept { return 0; }

	bzd::Size startUsageMonitoring() noexcept { return 0; }

	void getUsage() noexcept {}

	CoreId getId() noexcept { return 0; }
};

} // namespace bzd::platform::generic
