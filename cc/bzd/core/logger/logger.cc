#include "cc/bzd/core/logger/logger.h"

namespace {
bzd::backend::Logger* globalBackend{nullptr};
}

void bzd::Logger::setMinimumLevel(const bzd::log::Level level) noexcept
{
	minLevel_ = level;
}

bzd::Logger& bzd::Logger::getDefault() noexcept
{
	static Logger logger;
	return logger;
}

void bzd::Logger::setBackend(bzd::backend::Logger& backend)
{
	globalBackend = &backend;
}

void bzd::Logger::printHeader(const bzd::log::Level) noexcept
{
	if (!globalBackend)
	{
		return;
	}
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
