#include "cc/bzd/container/string.hh"
#include "cc/bzd/container/string_view.hh"
#include "cc/bzd/core/assert/minimal.hh"
#include "cc/bzd/core/logger.hh"
#include "cc/bzd/platform/panic.hh"

namespace bzd::assert::impl {

void backend(const bzd::SourceLocation& location, const char* message1, const char* message2)
{
	bzd::platform::panic([&](bzd::OStream& out) {
		if (message2 == nullptr)
		{
			bzd::Logger{out}.error("{}"_csv, message1, location).sync();
		}
		else
		{
			bzd::Logger{out}.error("{}{}"_csv, message1, message2, location).sync();
		}
	});
}

} // namespace bzd::assert::impl

namespace bzd::assert {
void unreachable(const bzd::SourceLocation location) { impl::backend(location, "Code unreachable.\n"); }
} // namespace bzd::assert
