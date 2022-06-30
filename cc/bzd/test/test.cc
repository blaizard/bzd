#include "cc/bzd/test/test.hh"

#include "cc/bzd/core/print.hh"
#include "cc/bzd/platform/clock.hh"

// Empty namespace to hold all the registered tests
namespace {

bzd::test::Test* testRoot{nullptr};
bzd::test::Test** testCurrent{&testRoot};

} // namespace

namespace bzd::test {

Test::Test(const char* testCaseName, const char* testName, const char* file) : testCaseName_{testCaseName}, testName_{testName}, file_{file}
{
	*testCurrent = this;
	testCurrent = &next_;
}

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
	auto* test = testRoot;

	::bzd::print("[==========] Running test(s)\n"_csv).sync();
	if (!test)
	{
		::bzd::print("[   FAILED ] Empty test suite is considered as a failed test.\n"_csv).sync();
		return false;
	}

	while (test)
	{
		const auto seed = test->getSeed();
		bzd::test::Context context{seed};

		::bzd::print("[ RUN      ] {}.{} (seed={})\n"_csv, test->testCaseName_, test->testName_, seed).sync();
		currentTestFailed_ = false;
		const auto tickStart = clock.getTicks();
		try
		{
			test->test(context);
		}
		catch (...)
		{
			fail(test->file_, -1, "Unknown C++ exception thrown in the test body.");
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

		test = test->next_;
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
