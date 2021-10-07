#include "tools/cc_test/test_internal.hh"

int main()
{
	extern void execute() noexcept;
	execute();

	if (::bzd::test::Manager::getInstance().run())
	{
		return 0;
	}
	return 1;
}
