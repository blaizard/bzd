#include "cc/bzd/core/assert/minimal.h"

#include "cc/bzd/container/string.h"
#include "cc/bzd/container/string_view.h"
#include "cc/bzd/core/logger.h"
#include "cc/bzd/platform/panic.h"
//#include "cc/bzd/utility/format/integral.h"

namespace bzd::assert::impl {

void backend(const bzd::SourceLocation& location, const char* message1, const char* message2)
{
	if (message2 == nullptr)
	{
		bzd::log::error(CSTR("{}"), message1, location).sync();
	}
	else
	{
		bzd::log::error(CSTR("{}{}"), message1, message2, location).sync();
	}
	bzd::platform::panic();
}
} // namespace bzd::assert::impl

namespace bzd::assert {
void unreachable(const bzd::SourceLocation location)
{
	impl::backend(location, "Code unreachable.\n");
}

} // namespace bzd::assert
