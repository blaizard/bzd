#pragma once

#include "cc/bzd/container/string.h"
#include "cc/bzd/container/string_view.h"
#include "cc/bzd/core/channel.h"
#include "cc/bzd/platform/out.h"
#include "cc/bzd/utility/format/format.h"
#include "cc/bzd/utility/forward.h"

namespace bzd::log {
enum class Level
{
	CRITICAL = 0,
	ERROR = 1,
	WARNING = 2,
	INFO = 3,
	DEBUG = 4
};

template <class... Args>
void print(Args&&... args) noexcept
{
	bzd::format::toStream(bzd::platform::getOut(), bzd::forward<Args>(args)...);
}
} // namespace bzd::log

namespace bzd::impl {
class Log
{
public:
	constexpr explicit Log(bzd::OChannel& out) noexcept : out_(out) {}

	template <class... Args>
	constexpr void info(Args&&... args) noexcept
	{
		print(bzd::log::Level::INFO, bzd::forward<Args>(args)...);
	}

private:
	void printHeader(const bzd::log::Level level)
	{
		bzd::StringView str;
		switch (level)
		{
		case bzd::log::Level::CRITICAL:
			str = "[c] ";
			break;
		case bzd::log::Level::ERROR:
			str = "[e] ";
			break;
		case bzd::log::Level::WARNING:
			str = "[w] ";
			break;
		case bzd::log::Level::INFO:
			str = "[i] ";
			break;
		case bzd::log::Level::DEBUG:
			str = "[d] ";
			break;
		default:
			bzd::assert::unreachable();
		}
		out_.write(str.asBytes());
	}

	template <class... Args>
	constexpr void print(const bzd::log::Level level, Args&&... args) noexcept
	{
		printHeader(level);
		bzd::format::toStream(out_, bzd::forward<Args>(args)...);
	}

	bzd::OChannel& out_;
};
} // namespace bzd::impl

namespace bzd::interface {
using Log = impl::Log;
}

namespace bzd {
class Log : public interface::Log
{
public:
	constexpr Log(bzd::OChannel& out) : interface::Log(out) {}
};
} // namespace bzd
