#pragma once

#include "cc/bzd/container/string_view.h"

namespace bzd::minimal::log {
/**
 * Set an error log entry using the default logger.
 */
void error(const bzd::StringView message, const SourceLocation location = SourceLocation::current()) noexcept;

/**
 * Set a warning log entry using the default logger.
 */
void warning(const bzd::StringView message, const SourceLocation location = SourceLocation::current()) noexcept;

/**
 * Set an informative log entry using the default logger.
 */
void info(const bzd::StringView message, const SourceLocation location = SourceLocation::current()) noexcept;

/**
 * Set a debug log entry using the default logger.
 */
void debug(const bzd::StringView message, const SourceLocation location = SourceLocation::current()) noexcept;
} // namespace bzd::minimal::log
