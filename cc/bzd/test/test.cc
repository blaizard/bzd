#include "cc/bzd/test/test.hh"

#include <cstring>
#include <iostream>
#include <map>
#include <math.h>
#include <sstream>
#include <vector>

// Empty namespace to hold all the registered tests
namespace {
[[nodiscard]] auto& getTestsSingleton()
{
	static ::std::map<const char*, ::std::map<const char*, bzd::test::Manager::TestInfo>> tests;
	return tests;
}
} // namespace

namespace bzd::test {
void Manager::failInternals(const char* const file, const int line, const char* const message, const char* actual, const char* expected)
{
	currentTestFailed_ = true;
	::std::cout << file;
	if (line > -1)
	{
		::std::cout << ":" << std::dec << line;
	}

	::std::cout << ": " << message << "\n";
	if (actual)
	{
		::std::cout << "Actual: " << actual << "\n";
	}
	if (expected)
	{
		::std::cout << "Expected: " << expected << "\n";
	}

	::std::cout << "Assertion failed.\n" << ::std::endl;
}

bool Manager::registerTest(Manager::TestInfo&& info)
{
	auto& tests = getTestsSingleton();
	{
		auto it = tests.find(info.testCaseName_);
		if (it == tests.end())
		{
			tests.emplace(info.testCaseName_, std::map<const char*, bzd::test::Manager::TestInfo>{});
		}
	}
	{
		auto it = tests[info.testCaseName_].find(info.testName_);
		if (it != tests[info.testCaseName_].end())
		{
			throw 42;
		}
		tests[info.testCaseName_].emplace(info.testName_, std::move(info));
	}
	return true;
}

bool bzd::test::Manager::run()
{
	::std::vector<const TestInfo*> failedTests;
	auto& tests = getTestsSingleton();

	::std::cout << "[==========] Running test(s) from " << std::dec << tests.size() << " test case(s)" << ::std::endl;
	if (tests.empty())
	{
		::std::cout << "[   FAILED ] Empty test suite is considered as a failed test." << ::std::endl;
		return false;
	}

	for (const auto& it1 : tests)
	{
		const auto testCaseName = it1.first;
		const auto nbTests = it1.second.size();
		::std::cout << "[----------] " << nbTests << " test(s) from " << testCaseName << ::std::endl;

		for (const auto& it2 : it1.second)
		{
			const auto testName = it2.first;
			const auto& info = it2.second;
			bzd::test::Context context{};

			::std::cout << "[ RUN      ] " << testCaseName << "." << testName << " (seed=" << context.getSeed() << ")" << ::std::endl;
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
				::std::cout << "[   FAILED ] " << ::std::endl;
			}
			else
			{
				::std::cout << "[       OK ] " << ::std::endl;
			}
		}

		::std::cout << "[----------] " << ::std::endl;
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
