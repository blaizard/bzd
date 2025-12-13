#include "cc/bzd/platform/panic.hh"
#include "cc/components/std/stream/out/out.hh"

#include <exception>

void __attribute__((weak)) bzd::platform::panic(const bzd::FunctionRef<void(bzd::OStream&)> callback)
{
	::bzd::components::std::Out out{};
	callback(out);
	// Triggers a signal on POSIX which provoques a stack trace dump.
	std::terminate();
}

int main()
{
	extern bool runExecutor() noexcept;
	if (runExecutor())
	{
		return 0;
	}
	return 1;
}
