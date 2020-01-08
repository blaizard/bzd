#include "bzd/core/assert.h"

namespace bzd { namespace assert {

void unreachable()
{
	bzd::getOut().write(bzd::StringView{"Code unreachable"});
	bzd::panic();
}

}} // namespace bzd::assert
