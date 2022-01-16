#include "cc/bzd.hh"
#include "cc/components/std/stream/out/out.hh"

int main()
{
	extern bool execute() noexcept;
	execute();

#if defined(BZD_EXECUTOR_SIM)
	{
		bzd::platform::std::Out{}.write("==== Termination of simulation ===="_sv.asBytes()).sync();
		::std::terminate();
	}
#endif

	// Prevent the program to exit.
	while (true)
	{
	};

	return 0;
}
