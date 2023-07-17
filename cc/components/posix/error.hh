#pragma once

#include "cc/bzd/container/result.hh"
#include "cc/bzd/core/error.hh"

#include <errno.h>
#include <string.h>

namespace bzd::error {
/// Generate an error from a POSIX API and feed it with debug information.
struct Errno : public bzd::ResultError<bzd::Error>
{
	// NOLINTNEXTLINE(bugprone-exception-escape)
	Errno(const bzd::StringView function, int errorCode = errno, const SourceLocation location = SourceLocation::current()) noexcept :
		bzd::ResultError<bzd::Error>
	{
		location, ErrorType::failure, "'{}', errno {}, '{}'"_csv, function, errorCode, ::strerror(errorCode)
	}
	{
	}
};
} // namespace bzd::error
