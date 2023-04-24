#include "cc/bzd/platform/panic.hh"
#include "cc/components/std/stream/out/out.hh"

#include <esp_system.h>

#ifdef BZD_EXECUTOR_SIM
#include <iostream>
#endif

void bzd::platform::panic(const bzd::FunctionRef<void(bzd::OStream&)> callback)
{
	::bzd::platform::std::Out out{};
	callback(out);
	::std::cout << "Infinite loop..." << ::std::endl;
	while (true)
	{
	};
}

int main()
{
	extern bool runExecutor() noexcept;
	[[maybe_unused]] const auto result = runExecutor();

#ifdef BZD_EXECUTOR_SIM
	::std::cout << "<simulation exit code " << ((result) ? 0 : 1) << ">" << ::std::endl;
	esp_restart();
#endif

	// Prevent the program to exit.
	while (true)
	{
	};

	return 0;
}
