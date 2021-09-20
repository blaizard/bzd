#include "cc/bzd/core/assert/minimal.h"

#include "cc/bzd/container/string.h"
#include "cc/bzd/container/string_view.h"
#include "cc/bzd/core/logger/minimal.h"
#include "cc/bzd/platform/panic.h"
//#include "cc/bzd/utility/format/integral.h"

namespace bzd::assert::impl {

void backend(const bzd::SourceLocation& location, const char* message1, const char* /*message2*/)
{
	bzd::String<10> str;
	// bzd::format::toString(str, location.getLine());
	bzd::minimal::Logger::getDefault().info(bzd::StringView{location.getFile()},
											bzd::StringView{":"},
											bzd::StringView{str.data(), str.size()},
											bzd::StringView{": "},
											bzd::StringView{message1});
	/*if (message2)
	{
		out.write(bzd::StringView{message2}.asBytes());
	}*/
	bzd::platform::panic();
}
} // namespace bzd::assert::impl

namespace bzd::assert {
void unreachable(const bzd::SourceLocation location)
{
	impl::backend(location, "Code unreachable.\n");
}

} // namespace bzd::assert
