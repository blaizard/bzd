#include "cc/bzd/test/test.hh"

#include "cc/bzd/core/print.hh"

#include <map>
#include <vector>

// Empty namespace to hold all the registered tests
namespace {

struct TestID
{
	const char* testCaseName;
	const char* testName;

	constexpr bool operator<(const TestID& other) const noexcept
	{
		auto result = bzd::test::impl::strcmp(testCaseName, other.testCaseName);
		if (result == 0)
		{
			result = bzd::test::impl::strcmp(testName, other.testName);
		}
		return (result == -1);
	}

	/// Generates a deterministic seed from the test identifier.
	constexpr auto getSeed() const noexcept
	{
		::bzd::test::Context::SeedType seed{362437};

		for (auto* it : {testCaseName, testName})
		{
			while (*it)
			{
				seed += *it;

				// Algorithm "xor" from p. 4 of Marsaglia, "Xorshift RNGs"
				// See: https://en.wikipedia.org/wiki/Xorshift
				seed ^= seed << 13;
				seed ^= seed >> 17;
				seed ^= seed << 5;

				++it;
			}
		}

		return seed;
	}
};

[[nodiscard]] auto& getTestsSingleton()
{
	static ::std::map<::TestID, bzd::test::Manager::TestInfo> tests;
	return tests;
}
} // namespace

namespace bzd::test {
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

bool Manager::registerTest(Manager::TestInfo&& info)
{
	auto& tests = getTestsSingleton();
	::TestID id{info.testCaseName_, info.testName_};

	auto it = tests.find(id);
	if (it != tests.end())
	{
		throw 42;
	}
	tests.emplace(id, std::move(info));

	return true;
}

bool bzd::test::Manager::run()
{
	::std::vector<const TestInfo*> failedTests;
	auto& tests = getTestsSingleton();

	::bzd::print(CSTR("[==========] Running test(s) from {} test case(s)\n"), tests.size()).sync();
	if (tests.empty())
	{
		::bzd::print(CSTR("[   FAILED ] Empty test suite is considered as a failed test.\n")).sync();
		return false;
	}

	for (const auto& it : tests)
	{
		const auto testCaseName = it.first.testCaseName;
		const auto testName = it.first.testName;
		const auto seed = it.first.getSeed();
		const auto& info = it.second;
		bzd::test::Context context{seed};

		::bzd::print(CSTR("[ RUN      ] {}.{} (seed={})\n"), testCaseName, testName, seed).sync();
		currentTestFailed_ = false;
		try
		{
			info.test_->test(context);
		}
		catch (...)
		{
			fail(info.file_, -1, "Unknown C++ exception thrown in the test body.");
		}

		// Print the test status
		if (currentTestFailed_)
		{
			failedTests.push_back(&info);
			::bzd::print(CSTR("[   FAILED ]\n")).sync();
		}
		else
		{
			::bzd::print(CSTR("[       OK ]\n")).sync();
		}
	}

	return (failedTests.empty()) ? true : false;
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
