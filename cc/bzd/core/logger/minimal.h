#pragma once

#include "cc/bzd/utility/source_location.h"

namespace bzd::minimal::log {
/**
 * Set an error log entry using the default logger.
 */
void error(const char* message, const SourceLocation location = SourceLocation::current()) noexcept;

/**
 * Set a warning log entry using the default logger.
 */
void warning(const char* message, const SourceLocation location = SourceLocation::current()) noexcept;

/**
 * Set an informative log entry using the default logger.
 */
void info(const char* message, const SourceLocation location = SourceLocation::current()) noexcept;

/**
 * Set a debug log entry using the default logger.
 */
void debug(const char* message, const SourceLocation location = SourceLocation::current()) noexcept;
} // namespace bzd::minimal::log
