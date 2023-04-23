#include "cc/bzd/test/test.hh"

#include "cc/bzd/core/print.hh"
#include "cc/bzd/platform/clock.hh"

// Empty namespace to hold all the registered tests
namespace {
bzd::test::TestNode nodeRoot{};
bzd::OStream* maybeOut{};
} // namespace

namespace bzd::test {

bzd::test::TestNode* nodeCurrent{&nodeRoot};

void Manager::failInternals(
	const char* const file, const bzd::Int32 line, const char* const message, const char* actual, const char* expected)
{
	auto& out = *::maybeOut;
	auto scope = out.getLock().sync();

	currentTestFailed_ = true;
	::bzd::printNoLock(out, "[----------] {}"_csv, file).sync();

	if (line > -1)
	{
		::bzd::printNoLock(out, ":{}"_csv, line).sync();
	}

	::bzd::printNoLock(out, ": {}\n"_csv, message).sync();
	if (actual)
	{
		::bzd::printNoLock(out, "Actual: {}\n"_csv, actual).sync();
	}
	if (expected)
	{
		::bzd::printNoLock(out, "Expected: {}\n"_csv, expected).sync();
	}

	::bzd::printNoLock(out, "Assertion failed.\n").sync();
}

bool bzd::test::Manager::run(bzd::OStream& out, bzd::SteadyClock& steadyClock)
{
	bzd::Size nbFailedTests{0};
	auto* node = &nodeRoot;

	::bzd::print(out, "[==========] Running test(s)\n"_csv).sync();
	if (!node->info)
	{
		::bzd::print(out, "[   FAILED ] Empty test suite is considered a failed test.\n"_csv).sync();
		return false;
	}

	::maybeOut = &out;
	while (node->isValid())
	{
		const auto seed = node->info->getSeed();
		bzd::test::Context context{seed};

		::bzd::print(out, "[ RUN      ] {}.{}"_csv, node->info->testCaseName, node->info->testName).sync();
		if (node->variant.size())
		{
			::bzd::print(out, ".{}"_csv, node->variant).sync();
		}
		::bzd::print(out, " (seed={})\n"_csv, seed).sync();

		currentTestFailed_ = false;
		const auto maybeTimeStart = steadyClock.getTime();
		try
		{
			node->function(context);
		}
		catch (...)
		{
			fail(node->info->file, -1, "Unknown C++ exception thrown in the test body.");
		}
		const auto maybeTimeStop = steadyClock.getTime();
		const auto timeDiffMs = (maybeTimeStart && maybeTimeStop) ? (maybeTimeStop.value() - maybeTimeStart.value()).get() : -1;

		// Print the test status
		if (currentTestFailed_)
		{
			++nbFailedTests;
			::bzd::print(out, "[   FAILED ] ({}ms)\n"_csv, timeDiffMs).sync();
		}
		else
		{
			::bzd::print(out, "[       OK ] ({}ms)\n"_csv, timeDiffMs).sync();
		}

		node = node->next;
	}
	::maybeOut = nullptr;

	return (nbFailedTests == 0);
}

bzd::Async<bool> run(bzd::OStream& out, bzd::SteadyClock& steadyClock)
{
	if (::bzd::test::Manager::getInstance().run(out, steadyClock))
	{
		co_return true;
	}
	co_return ::bzd::error::Failure("Test failed."_csv);
}

} // namespace bzd::test
