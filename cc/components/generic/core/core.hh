#pragma once

#include "cc/bzd/container/optional.hh"
#include "cc/bzd/container/result.hh"
#include "cc/bzd/container/stack.hh"
#include "cc/components/generic/core/interface.hh"

namespace bzd::platform::generic {

class Core : public bzd::platform::adapter::Core<Core>
{
private:
	using Self = Core;
	using Parent = bzd::platform::adapter::Core<Self>;
	using Error = typename Parent::Error;

public:
	explicit Core() noexcept {}

	void init() {}

	Result<void, Error> stop() noexcept { return bzd::nullresult; }

	bzd::Result<void, Error> start(bzd::platform::WorkloadType workload) noexcept
	{
		workload();
		return bzd::nullresult;
	}

	StackSize getStackUsage() noexcept { return 0; }

	bzd::SizeType startUsageMonitoring() noexcept { return 0; }

	void getUsage() noexcept {}

	CoreId getId() noexcept { return 0; }
};

} // namespace bzd::platform::generic
