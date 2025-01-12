#pragma once

#include "cc/bzd/platform/types.hh"
#include "cc/components/generic/executor_profiler/noop/core_profiler.hh"

namespace bzd::components::generic {

template <class Context = void>
class ExecutorProfilerNoop
{
public:
	ExecutorProfilerNoop() = default;
	constexpr ExecutorProfilerNoop(auto&) noexcept {}
	constexpr CoreProfilerNoop makeCoreProfiler() const noexcept { return CoreProfilerNoop{}; }
};

} // namespace bzd::components::generic
