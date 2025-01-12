#include "cc/components/generic/executor_profiler/noop/core_profiler.hh"

namespace bzd::components::generic {

CoreProfilerNoop& getCoreProfilerNoop() noexcept
{
	static CoreProfilerNoop profiler{};
	return profiler;
}

} // namespace bzd::components::generic
