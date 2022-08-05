#include "cc/bzd/test/test.hh"

#include "cc/bzd/core/print.hh"
#include "cc/bzd/platform/clock.hh"

// Empty namespace to hold all the registered tests
namespace {
bzd::test::TestNode nodeRoot{};
}

namespace bzd::test {

bzd::test::TestNode* nodeCurrent{&nodeRoot};

void Manager::failInternals(
	const char* const file, const bzd::Int32 line, const char* const message, const char* actual, const char* expected)
{
	auto& out = ::bzd::platform::out();
	auto scope = out.getLock().sync();

	currentTestFailed_ = true;
	::bzd::printNoLock("[----------] {}"_csv, file).sync();

	if (line > -1)
	{
		::bzd::printNoLock(":{}"_csv, line).sync();
	}

	::bzd::printNoLock(": {}\n"_csv, message).sync();
	if (actual)
	{
		::bzd::printNoLock("Actual: {}\n"_csv, actual).sync();
	}
	if (expected)
	{
		::bzd::printNoLock("Expected: {}\n"_csv, expected).sync();
	}

	::bzd::printNoLock("Assertion failed.\n").sync();
}

bool bzd::test::Manager::run()
{
	auto& clock = ::bzd::platform::steadyClock();
	bzd::Size nbFailedTests{0};
	auto* node = &nodeRoot;

	::bzd::print("[==========] Running test(s)\n"_csv).sync();
	if (!node->info)
	{
		::bzd::print("[   FAILED ] Empty test suite is considered as a failed test.\n"_csv).sync();
		return false;
	}

	while (node->isValid())
	{
		const auto seed = node->info->getSeed();
		bzd::test::Context context{seed};

		::bzd::print("[ RUN      ] {}.{}"_csv, node->info->testCaseName, node->info->testName).sync();
		if (node->variant.size())
		{
			::bzd::print(".{}"_csv, node->variant).sync();
		}
		::bzd::print(" (seed={})\n"_csv, seed).sync();

		currentTestFailed_ = false;
		const auto tickStart = clock.getTicks();
		try
		{
			node->function(context);
		}
		catch (...)
		{
			fail(node->info->file, -1, "Unknown C++ exception thrown in the test body.");
		}

		const auto tickDiff = (clock.getTicks() - tickStart);

		// Print the test status
		if (currentTestFailed_)
		{
			++nbFailedTests;
			::bzd::print("[   FAILED ] ({}ms)\n"_csv, clock.ticksToMs(tickDiff).get()).sync();
		}
		else
		{
			::bzd::print("[       OK ] ({}ms)\n"_csv, clock.ticksToMs(tickDiff).get()).sync();
		}

		node = node->next;
	}

	return (nbFailedTests == 0);
}

bzd::Async<bool> run()
{
	if (::bzd::test::Manager::getInstance().run())
	{
		co_return true;
	}
	co_return ::bzd::error::Failure("Test failed."_csv);
}

} // namespace bzd::test
