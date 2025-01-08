#include "cc/bzd/test/test.hh"

#include "cc/bzd/core/print.hh"

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

bzd::Async<bool> bzd::test::Manager::run(bzd::OStream& out, bzd::Timer& timer, bzd::test::Runner& runner)
{
	bzd::Size nbFailedTests{0};
	auto* node = &nodeRoot;

	co_await !bzd::print(out, "[==========] Running test(s)\n"_csv);
	if (!node->info)
	{
		co_await !bzd::print(out, "[   FAILED ] Empty test suite is considered a failed test.\n"_csv);
		co_return false;
	}

	::maybeOut = &out;
	while (node->isValid())
	{
		const auto seed = node->info->getSeed();
		bzd::test::Context context{seed, timer, runner};

		co_await !::bzd::print(out, "[ RUN      ] {}.{}"_csv, node->info->testCaseName, node->info->testName);
		if (node->variant.size())
		{
			co_await !::bzd::print(out, ".{}"_csv, node->variant);
		}
		co_await !::bzd::print(out, " (seed={})\n"_csv, seed);

		currentTestFailed_ = false;
		const auto maybeTimeStart = timer.getTime();
		try
		{
			if (node->function.isAsync())
			{
				co_await !node->function.runAsync(context);
			}
			else
			{
				node->function.run(context);
			}
		}
		catch (...)
		{
			fail(node->info->file, -1, "Unknown C++ exception thrown in the test body.");
		}
		const auto maybeTimeStop = timer.getTime();
		const auto timeDiffMs = (maybeTimeStart && maybeTimeStop) ? (maybeTimeStop.value() - maybeTimeStart.value()).get() : -1;

		// Print the test status
		if (currentTestFailed_)
		{
			++nbFailedTests;
			co_await !::bzd::print(out, "[   FAILED ] ({}ms)\n"_csv, timeDiffMs);
		}
		else
		{
			co_await !::bzd::print(out, "[       OK ] ({}ms)\n"_csv, timeDiffMs);
		}

		node = node->next;
	}
	::maybeOut = nullptr;

	co_return (nbFailedTests == 0);
}

} // namespace bzd::test
