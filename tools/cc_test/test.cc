#include "cc_test/test.hh"

#include <cstring>
#include <iostream>
#include <map>
#include <math.h>
#include <sstream>
#include <vector>

// Empty namespace to hold all the registered tests
namespace {
static ::std::map<const char*, ::std::map<const char*, bzd::test::Manager::TestInfo>> tests_;
}

namespace bzd::test::impl {
bool strcmp(const char* str1, const char* str2)
{
	return (::std::strcmp(str1, str2) == 0);
}
bool near(const double number1, const double number2, const double absError)
{
	const double diff = ::fabs(number1 - number2);
	return (diff <= absError);
}
} // namespace bzd::test::impl

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
	{
		auto it = tests_.find(info.testCaseName_);
		if (it == tests_.end())
		{
			tests_.emplace(info.testCaseName_, std::map<const char*, bzd::test::Manager::TestInfo>{});
		}
	}
	{
		auto it = tests_[info.testCaseName_].find(info.testName_);
		if (it != tests_[info.testCaseName_].end())
		{
			throw 42;
		}
		tests_[info.testCaseName_].emplace(info.testName_, std::move(info));
	}
	return true;
}

bool bzd::test::Manager::run()
{
	::std::vector<const TestInfo*> failedTests;

	::std::cout << "[==========] Running test(s) from " << std::dec << tests_.size() << " test case(s)" << ::std::endl;

	for (const auto& it1 : tests_)
	{
		const auto testCaseName = it1.first;
		const auto nbTests = it1.second.size();
		::std::cout << "[----------] " << nbTests << " test(s) from " << testCaseName << ::std::endl;

		for (const auto& it2 : it1.second)
		{
			const auto testName = it2.first;
			const auto& info = it2.second;
			::std::cout << "[ RUN      ] " << testCaseName << "." << testName << ::std::endl;
			currentTestFailed_ = false;
			try
			{
				info.test_->test();
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

} // namespace bzd::test

int main()
{
	if (::bzd::test::Manager::getInstance().run())
	{
		return 0;
	}
	return 1;
}
