#pragma once

#include "cc/bzd/core/logger/backend/logger.h"

namespace bzd::log {
enum class Level
{
	CRITICAL = 0,
	ERROR = 1,
	WARNING = 2,
	INFO = 3,
	DEBUG = 4
};
}

namespace bzd {
class Logger
{
public:
	/**
	 * Set the minimum logging level to be displayed.
	 */
	void setMinimumLevel(const bzd::log::Level level) noexcept;

	template <class... Args>
	constexpr void info(Args&&... args) noexcept
	{
		print(bzd::log::Level::INFO, bzd::forward<Args>(args)...);
	}

	/**
	 * Get the default Logger.
	 */
	[[nodiscard]] static Logger& getDefault() noexcept;

protected:
	/**
	 * Set the logger backend, by default a stub is in place.
	 * The logger backend is global for the whole application.
	 */
	static void setBackend(bzd::backend::Logger& backend);

private:
	void printHeader(const bzd::log::Level level) noexcept;

	template <class... Args>
	constexpr void print(const bzd::log::Level level, Args&&...) noexcept
	{
		if (level <= minLevel_)
		{
			// printHeader(level);
			// bzd::format::toStream(out_, bzd::forward<Args>(args)...);
		}
	}

private:
	bzd::log::Level minLevel_{bzd::log::Level::INFO};
};

} // namespace bzd
