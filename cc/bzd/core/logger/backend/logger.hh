#pragma once

#include "cc/bzd/container/optional.hh"
#include "cc/bzd/container/reference_wrapper.hh"
#include "cc/bzd/core/channel.hh"

namespace bzd::backend {

class Logger
{
public:
	/**
	 * Set the default logger backend, by default a stub is in place.
	 * Return the previous backend if any.
	 */
	static bzd::Optional<bzd::OStream&> setDefault(bzd::OStream& backend) noexcept;

	/**
	 * Get the default backend logger.
	 */
	static bzd::OStream& getDefault() noexcept;
};

} // namespace bzd::backend
