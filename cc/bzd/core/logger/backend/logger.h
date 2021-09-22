#pragma once

#include "cc/bzd/core/channel.h"

namespace bzd::backend {

class Logger : public bzd::OStream
{
public:
	/**
	 * Set the default logger backend, by default a stub is in place.
	 */
	static void setDefault(bzd::backend::Logger& backend) noexcept;

	/**
	 * Get the default backend logger.
	 */
	static bzd::backend::Logger& getDefault() noexcept;
};

} // namespace bzd::backend
