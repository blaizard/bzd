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

void Manager::failInternals(const char* const file, const int line, const char* const message, const char* actual, const char* expected)
{
	auto& out = ::bzd::platform::out();
	auto scope = out.getLock().sync();

	currentTestFailed_ = true;
	::bzd::printNoLock(CSTR("[----------] {}"), file).sync();

	if (line > -1)
	{
		::bzd::printNoLock(CSTR(":{}"), line).sync();
	}

	::bzd::printNoLock(CSTR(": {}\n"), message).sync();
	if (actual)
	{
		::bzd::printNoLock(CSTR("Actual: {}\n"), actual).sync();
	}
	if (expected)
	{
		::bzd::printNoLock(CSTR("Expected: {}\n"), expected).sync();
	}

	::bzd::printNoLock("Assertion failed.\n").sync();
}

bool bzd::test::Manager::run()
{
	auto& clock = ::bzd::platform::steadyClock();
	bzd::SizeType nbFailedTests{0};
	auto* test = testRoot;

	::bzd::print(CSTR("[==========] Running test(s)\n")).sync();
	if (!test)
	{
		::bzd::print(CSTR("[   FAILED ] Empty test suite is considered as a failed test.\n")).sync();
		return false;
	}

	while (test)
	{
		const auto seed = test->getSeed();
		bzd::test::Context context{seed};

		::bzd::print(CSTR("[ RUN      ] {}.{} (seed={})\n"), test->testCaseName_, test->testName_, seed).sync();
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
			::bzd::print(CSTR("[   FAILED ] ({}ms)\n"), clock.ticksToMs(tickDiff).get()).sync();
		}
		else
		{
			::bzd::print(CSTR("[       OK ] ({}ms)\n"), clock.ticksToMs(tickDiff).get()).sync();
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
	co_return ::bzd::error(::bzd::ErrorType::failure, CSTR("Test failed."));
}

} // namespace bzd::test
