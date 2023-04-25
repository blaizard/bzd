#include "cc/bzd/platform/panic.hh"
#include "cc/components/std/stream/out/out.hh"

#include <atomic>
#include <exception>
#include <thread>

void bzd::platform::panic(const bzd::FunctionRef<void(bzd::OStream&)> callback)
{
	::bzd::components::std::Out out{};
	callback(out);
	// Triggers a signal on POSIX which provoques a stack trace dump.
	std::terminate();
}

int main()
{
	extern bool runExecutor() noexcept;
	extern bool runSecondExecutor() noexcept;
	std::atomic<bool> isSuccess{true};

	std::thread threadExecutor{[&isSuccess] {
		if (!runExecutor())
		{
			isSuccess = false;
		}
	}};
	std::thread threadSecondExecutor{[&isSuccess] {
		if (!runSecondExecutor())
		{
			isSuccess = false;
		}
	}};

	threadExecutor.join();
	threadSecondExecutor.join();

	return (isSuccess) ? 0 : 1;
}
