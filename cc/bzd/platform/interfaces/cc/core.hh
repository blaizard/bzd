#pragma once

#include "cc/bzd/platform/types.hh"
#include "tools/bdl/generators/cc/adapter/types.hh"

namespace bzd::platform {

// Type describing the size of a stack.
using StackSize = bzd::UInt32;
// Type describing the identifier of a core.
using CoreId = bzd::UInt8;

class Core
{
public:
	virtual ~Core() = default;

public:
	// Start a workload on the core and return immediatly.
	[[nodiscard]] virtual bzd::Result<void, bzd::Error> start(const bzd::FunctionRef<void(Core&)> workload) noexcept = 0;

	// Initialize the core.
	[[nodiscard]] virtual bzd::Result<void, bzd::Error> stop() noexcept = 0;

	// Get the current stack usage.
	[[nodiscard]] virtual bzd::platform::StackSize getStackUsage() noexcept = 0;

	// Get the current core identifier.
	[[nodiscard]] virtual bzd::platform::CoreId getId() noexcept = 0;
};

} // namespace bzd::platform
