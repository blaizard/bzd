#pragma once

#include "cc/bzd/container/result.hh"
#include "cc/bzd/core/error.hh"

#include <errno.h>
#include <string.h>

namespace bzd::error {
/// Generate an error from a POSIX API and feed it with debug information.
struct Posix : public bzd::ResultError<bzd::Error>
{
	// NOLINTNEXTLINE(bugprone-exception-escape)
	Posix(const bzd::StringView function, const SourceLocation location = SourceLocation::current()) noexcept :
		bzd::ResultError<bzd::Error>{
			location, ErrorType::failure, "POSIX function '{}', errno {}, '{}'"_csv, function, errno, ::strerror(errno)}
	{
	}
};
} // namespace bzd::error
