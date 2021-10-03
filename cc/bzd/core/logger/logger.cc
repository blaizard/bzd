#include "cc/bzd/core/logger.hh"

#include "cc/bzd/core/logger/backend/logger.hh"
#include "cc/bzd/core/units.hh"
#include "cc/bzd/platform/clock.hh"

namespace {
const char* levelToStr(const bzd::log::Level level)
{
	switch (level)
	{
	case bzd::log::Level::ERROR:
		return "[e]";
	case bzd::log::Level::WARNING:
		return "[w]";
	case bzd::log::Level::INFO:
		return "[i]";
	case bzd::log::Level::DEBUG:
		return "[d]";
	default:
		bzd::assert::unreachable();
	}
	return "";
}
} // namespace

void bzd::Logger::setMinimumLevel(const bzd::log::Level level) noexcept
{
	minLevel_ = level;
}

bzd::Logger& bzd::Logger::getDefault() noexcept
{
	static Logger logger;
	return logger;
}

struct Date
{
	bzd::units::Millisecond ms_;
};

bzd::Async<void> toStream(bzd::OStream& os, const Date& d)
{
	const auto ms = d.ms_.get() % 1000;
	const auto s = bzd::units::Second{d.ms_}.get() % 60;
	const auto min = bzd::units::Minute{d.ms_}.get() % 60;
	const auto h = bzd::units::Hour{d.ms_}.get() % 24;
	// const auto d = bzd::units::Day{d.ms_}.get() % ;
	// const auto m = bzd::units::Hour{d.ms_}.get() % 24;
	// const auto y = bzd::units::Hour{d.ms_}.get() % 24;
	co_await bzd::format::toStream(os, CSTR("{:.2}:{:.2}:{:.2}.{:.4}"), int(h), int(min), int(s), int(ms));
}

bzd::Async<void> bzd::Logger::printHeader(const bzd::log::Level level, const SourceLocation location) noexcept
{
	auto& backend = bzd::backend::Logger::getDefault();
	/*	const auto ms = bzd::platform::systemClock().getMs();

		const Date date{ms};*/
	co_await bzd::format::toStream(backend, CSTR("{} [{}:{}] "), levelToStr(level), location.getFileName(), location.getLine());
}

void bzd::minimal::log::error(const char* message, const SourceLocation location) noexcept
{
	bzd::Logger::getDefault().error(message, location).sync();
}

void bzd::minimal::log::warning(const char* message, const SourceLocation location) noexcept
{
	bzd::Logger::getDefault().warning(message, location).sync();
}

void bzd::minimal::log::info(const char* message, const SourceLocation location) noexcept
{
	bzd::Logger::getDefault().info(message, location).sync();
}

void bzd::minimal::log::debug(const char* message, const SourceLocation location) noexcept
{
	bzd::Logger::getDefault().debug(message, location).sync();
}
