#pragma once

#define BZDTEST_CLASS_NAME_(testCaseName, testName) BzdTest_##testCaseName##_##testName
#define BZDTEST_REGISTER_NAME_(testCaseName, testName) registerBzdTest_##testCaseName##_##testName##_

#define BZDTEST_(testCaseName, testName) \
	class BZDTEST_CLASS_NAME_(testCaseName, testName) : public ::bzd::test::Test \
	{ \
	public: \
		BZDTEST_CLASS_NAME_(testCaseName, testName)() {} \
		void test() const override; \
	}; \
	namespace { \
	static auto BZDTEST_REGISTER_NAME_(testCaseName, testName) = ::bzd::test::Manager::getInstance().registerTest( \
		{#testCaseName, #testName, __FILE__, new BZDTEST_CLASS_NAME_(testCaseName, testName)}); \
	} \
	void BZDTEST_CLASS_NAME_(testCaseName, testName)::test() const

#define BZDTEST_FAIL_FATAL_(...) return ::bzd::test::Manager::getInstance().fail(__FILE__, __LINE__, __VA_ARGS__)
#define BZDTEST_FAIL_NONFATAL_(...) ::bzd::test::Manager::getInstance().fail(__FILE__, __LINE__, __VA_ARGS__)

#define BZDTEST_TEST_BOOLEAN_(condition, actual, expected, failFct) \
	if (!static_cast<bool>(condition)) failFct("Failure\nValue of: " #actual, static_cast<bool>(actual), static_cast<bool>(expected))

#define BZDTEST_TEST_EQ_(expression1, expression2, failFct) \
	if (!((expression1) == (expression2))) \
	failFct("Failure\nExpected: " #expression1 "\nTo be equal to: " #expression2 "\nAssertion failed.")

#define BZDTEST_TEST_NEAR_(number1, number2, absError, failFct) \
	if (!bzd::test::impl::near(number1, number2, absError)) \
	failFct("Failure\nExpected: " #number1 "\nTo be near to: " #number2 "\nAssertion failed.")

#define BZDTEST_TEST_STREQ_(str1, str2, failFct) \
	if (!bzd::test::impl::strcmp(str1, str2)) failFct("Failure\nExpected: " #str1 "\nTo be equal to: " #str2 "\nAssertion failed.")

#define BZDTEST_TEST_ANY_THROW_(expression, failFct)  { \
		bool bzdTestIsThrow_ = false; \
		try \
		{ \
			expression; \
		} \
		catch (...) \
		{ \
			bzdTestIsThrow_ = true; \
		} \
		if (!bzdTestIsThrow_) failFct("Failure\nExpected: " #expression " to throw but did not throw."); \
	}

namespace bzd { namespace test {

namespace impl {
bool strcmp(const char* str1, const char* str2);
bool near(const double number1, const double number2, const double absError);
} // namespace impl

class Test
{
public:
	Test() = default;
	virtual ~Test() {}
	virtual void test() const = 0;
};

class Manager
{
public:
	class TestPtr
	{
	public:
		TestPtr(const Test* test) : test_(test) {}
		TestPtr(TestPtr&& other) : test_(other.test_) { other.test_ = nullptr; }
		TestPtr& operator=(TestPtr&& other)
		{
			test_ = other.test_;
			other.test_ = nullptr;
			return *this;
		}

		~TestPtr()
		{
			if (test_) delete test_;
		}
		const Test* operator->() const { return test_; }

	private:
		const Test* test_;
	};

	struct TestInfo
	{
		const char* testCaseName_;
		const char* testName_;
		const char* const file_;
		TestPtr test_;
	};

public:
	static Manager& getInstance()
	{
		static Manager instance;
		return instance;
	}
	Manager(Manager const&) = delete;
	void operator=(Manager const&) = delete;

	bool registerTest(TestInfo&& test);
	bool run();
	void fail(const char* const file, const int line, const char* const message);
	void fail(const char* const file, const int line, const char* const message, const bool actual, const bool expected);

private:
	Manager() = default;
	bool currentTestFailed_ = false;
};

}} // namespace bzd::test
