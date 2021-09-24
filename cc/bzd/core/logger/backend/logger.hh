#pragma once

#include "cc/bzd/core/channel.hh"

namespace bzd::backend {

class Logger
{
public:
	/**
	 * Set the default logger backend, by default a stub is in place.
	 */
	static void setDefault(bzd::OStream& backend) noexcept;

	/**
	 * Get the default backend logger.
	 */
	static bzd::OStream& getDefault() noexcept;
};

} // namespace bzd::backend
