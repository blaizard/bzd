#pragma once

#include "cc/bzd/container/string_view.h"

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

namespace bzd::minimal {
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

private:
	void printHeader(const bzd::log::Level level) noexcept;

	template <class... Args>
	constexpr void print(const bzd::log::Level level, Args&&... args) noexcept
	{
		if (level <= minLevel_)
		{
			printHeader(level);
			printBody(bzd::forward<Args>(args)...);
		}
	}

	template <class T, class... Args>
	constexpr void printBody(const T&, Args&&... args) noexcept
	{
		if constexpr (sizeof...(Args) > 0)
		{
			printBody(bzd::forward<Args>(args)...);
		}
	}

protected:
	bzd::log::Level minLevel_{bzd::log::Level::INFO};
};

} // namespace bzd::minimal

namespace bzd::minimal::log {
template <class... Args>
constexpr void info(Args&&... args) noexcept
{
	bzd::minimal::Logger::getDefault().info(bzd::forward<Args>(args)...);
}
} // namespace bzd::minimal::log
