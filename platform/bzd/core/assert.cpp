#include "bzd/core/assert.h"

#include "bzd/core/system.h"

namespace bzd { namespace assert {

void unreachable()
{
	bzd::getOut().write(bzd::StringView{"Code unreachable"});
	bzd::panic();
}

}} // namespace bzd::assert
