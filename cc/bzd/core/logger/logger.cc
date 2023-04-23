#include "cc/bzd/core/logger.hh"

namespace {
const char* levelToStr(const bzd::log::Level level)
{
	switch (level)
	{
	case bzd::log::Level::error:
		return "[e]";
	case bzd::log::Level::warning:
		return "[w]";
	case bzd::log::Level::info:
		return "[i]";
	case bzd::log::Level::debug:
		return "[d]";
	default:
		bzd::assert::unreachable();
	}
	return "";
}
} // namespace

void bzd::Logger::setMinimumLevel(const bzd::log::Level level) noexcept { minLevel_ = level; }

/*
namespace {
struct Date
{
	bzd::units::Millisecond ms_;
};
}

bzd::Async<> toStream(bzd::OStream& os, const Date& d)
{
	const auto ms = d.ms_.get() % 1000;
	const auto s = bzd::units::Second{d.ms_}.get() % 60;
	const auto min = bzd::units::Minute{d.ms_}.get() % 60;
	const auto h = bzd::units::Hour{d.ms_}.get() % 24;
	// const auto d = bzd::units::Day{d.ms_}.get() % ;
	// const auto m = bzd::units::Hour{d.ms_}.get() % 24;
	// const auto y = bzd::units::Hour{d.ms_}.get() % 24;
	co_await bzd::format::toStream(os, "{:.2}:{:.2}:{:.2}.{:.4}"_csv, int(h), int(min), int(s), int(ms));
	co_return {};
}
*/

// NOLINTNEXTLINE(bugprone-exception-escape)
bzd::Async<> bzd::Logger::printHeader(const bzd::log::Level level, const SourceLocation location) noexcept
{
	co_await !toStream(stream_, "{} [{}:{}] "_csv, levelToStr(level), location.getFileName(), location.getLine());
	co_return {};
}
