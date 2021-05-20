#include "bzd/core/assert/minimal.h"

#include "bzd/container/string_view.h"
#include "bzd/core/channel.h"
#include "bzd/core/registry.h"
#include "bzd/platform/out.h"
#include "bzd/platform/panic.h"
#include "bzd/utility/format/integral.h"

namespace {
bzd::OChannel& getOChannel()
{
	if (bzd::Registry<bzd::OChannel>::is("stdout"))
	{
		return bzd::Registry<bzd::OChannel>::get("stdout");
	}
	return bzd::platform::getOut();
}
} // namespace

namespace bzd::assert::impl {

void backend(const bzd::SourceLocation& location, const char* message1, const char* message2)
{
	auto& out = getOChannel();
	out.write(bzd::StringView{location.getFile()}.asBytes());
	bzd::String<10> str;
	bzd::format::toStream(str, location.getLine());
	out.write(bzd::StringView{":"}.asBytes());
	out.write(str.asBytes());
	out.write(bzd::StringView{": "}.asBytes());
	out.write(bzd::StringView{message1}.asBytes());
	if (message2)
	{
		out.write(bzd::StringView{message2}.asBytes());
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
