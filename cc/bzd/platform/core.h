#pragma once

#include "cc/bzd/container/named_type.h"

namespace bzd::platform {

using CoreId = bzd::NamedType<bzd::UInt8Type, struct CoreId>;

/**
 * A core is a processing unit that accepts a single scheduler.
 */
class Core
{
public:
	constexpr explicit Core(const CoreId id) noexcept : id_{id} {}

	/**
	 * Initialize a core.
	 */
	void init() noexcept;

private:
	const CoreId id_{};
};

} // namespace bzd::platform
