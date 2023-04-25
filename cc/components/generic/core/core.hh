#pragma once

#include "cc/bzd/container/optional.hh"
#include "cc/bzd/container/result.hh"
#include "cc/bzd/container/stack.hh"
#include "cc/components/generic/core/interface.hh"

namespace bzd::components::generic {

class Core : public bzd::Core
{
private:
	using Self = Core;

public:
	template <class Context>
	constexpr explicit Core(Context&) noexcept
	{
	}

	void init() {}

	bzd::Result<void, bzd::Error> stop() noexcept override { return bzd::nullresult; }

	bzd::Result<void, bzd::Error> start(const bzd::FunctionRef<void(bzd::Core&)> workload) noexcept override
	{
		workload(*this);
		return bzd::nullresult;
	}

	StackSize getStackUsage() noexcept override { return 0; }

	bzd::Size startUsageMonitoring() noexcept { return 0; }

	void getUsage() noexcept {}

	CoreId getId() noexcept override { return 0; }
};

} // namespace bzd::components::generic
