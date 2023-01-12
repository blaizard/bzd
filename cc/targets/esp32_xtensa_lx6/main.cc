#include "cc/bzd.hh"

#include <esp_system.h>

#ifdef BZD_EXECUTOR_SIM
#include <iostream>
#endif

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
