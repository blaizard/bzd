#pragma once

#include "cc/bzd/core/async.h"
#include "cc/bzd/core/logger/backend/logger.h"
#include "cc/bzd/core/logger/minimal.h"
#include "cc/bzd/utility/format/format.h"

namespace bzd {
class Logger : public bzd::minimal::Logger
{
public:
	template <class... Args>
	Async<void> info(Args&&... args) noexcept
	{
		co_await print(bzd::log::Level::INFO, bzd::forward<Args>(args)...);
		co_return;
	}

	/**
	 * Get the default Logger.
	 */
	[[nodiscard]] static Logger& getDefault() noexcept;

private:
	Async<void> printHeader(const bzd::log::Level level) noexcept;

	template <class... Args>
	Async<void> print(const bzd::log::Level level, Args&&... args) noexcept
	{
		if (level <= minLevel_)
		{
			co_await printHeader(level);
			auto& backend = bzd::backend::Logger::getDefaultBackend();
			co_await bzd::format::toStream(backend, bzd::forward<Args>(args)...);
		}
	}
};

} // namespace bzd

namespace bzd::log {
template <class... Args>
Async<void> info(Args&&... args) noexcept
{
	co_await bzd::Logger::getDefault().info(bzd::forward<Args>(args)...);
}
} // namespace bzd::log
