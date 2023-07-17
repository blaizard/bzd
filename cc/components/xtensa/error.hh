#pragma once

#include "cc/bzd/container/result.hh"
#include "cc/bzd/core/error.hh"
#include "esp_err.h"

namespace bzd::error {
/// Generate an error from an esp_err and feed it with debug information.
struct EspErr : public bzd::ResultError<bzd::Error>
{
	// NOLINTNEXTLINE(bugprone-exception-escape)
	EspErr(const bzd::StringView function, ::esp_err_t errorCode, const SourceLocation location = SourceLocation::current()) noexcept :
		bzd::ResultError<bzd::Error>
	{
		location, ErrorType::failure, "'{}', code {}, '{}'"_csv, function, errorCode, ::esp_err_to_name(errorCode)
	}
	{
	}
};
} // namespace bzd::error
