#include "cc/bzd/core/logger/logger.h"

#include "cc/bzd/core/logger/backend/logger.h"

void bzd::minimal::Logger::setMinimumLevel(const bzd::log::Level level) noexcept
{
	minLevel_ = level;
}

bzd::minimal::Logger& bzd::minimal::Logger::getDefault() noexcept
{
	return bzd::Logger::getDefault();
}

bzd::Logger& bzd::Logger::getDefault() noexcept
{
	static Logger logger;
	return logger;
}

bzd::Async<void> bzd::Logger::printHeader(const bzd::log::Level level) noexcept
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
	auto& backend = bzd::backend::Logger::getDefaultBackend();
	co_await backend.write(str.asBytes());
}

void bzd::minimal::Logger::printHeader(const bzd::log::Level) noexcept
{
	/*
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
			break;bzd::Logger::
		default:
			bzd::assert::unreachable();
		}
		globalBackend->write(str.asBytes());*/
}

/*
void bzd::Logger::print(const bzd::StringView) noexcept
{

}
*/