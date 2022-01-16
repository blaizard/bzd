#include "cc/bzd.hh"

#include <esp_system.h>

int main()
{
	extern bool execute() noexcept;
	execute();

#if defined(BZD_EXECUTOR_SIM)
	esp_restart();
#endif

	// Prevent the program to exit.
	while (true)
	{
	};

	return 0;
}
