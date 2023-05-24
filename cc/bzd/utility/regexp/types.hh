#pragma once

#include "cc/bzd/container/result.hh"

namespace bzd::regexp {

enum class Success
{
	more,
	last,
};

enum class Error
{
	malformed,
	noMatch,
	noMoreInput,
};

using Result = bzd::Result<Success, Error>;

} // namespace bzd::regexp
