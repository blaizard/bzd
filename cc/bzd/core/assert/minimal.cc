#include "bzd/core/assert/minimal.h"

#include "bzd/container/string_view.h"
#include "bzd/core/channel.h"
#include "bzd/core/registry.h"
#include "bzd/platform/out.h"
#include "bzd/platform/panic.h"

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

void backend(const char* message1, const char* message2)
{
	auto& out = getOChannel();
	out.write(bzd::StringView{message1});
	if (message2)
	{
		out.write(bzd::StringView{message2});
	}
	bzd::platform::panic();
}
} // namespace bzd::assert::impl

namespace bzd::assert {
void unreachable()
{
	impl::backend("Code unreachable.\n");
}

} // namespace bzd::assert
