#pragma once

#include "cc/bzd/core/async.hh"

#include <random>

#define BZDTEST_FCT_NAME_(testCaseName, testName) functionBzdTest_##testCaseName##_##testName
#define BZDTEST_CLASS_NAME_(testCaseName, testName) BzdTest_##testCaseName##_##testName
#define BZDTEST_REGISTER_NAME_(testCaseName, testName) registerBzdTest_##testCaseName##_##testName##_
#define BZDTEST_COMPILE_TIME_FCT_NAME_(testCaseName, testName) compileTimeFunctionBzdTest_##testCaseName##_##testName

#define BZDTEST_REGISTER_(testCaseName, testName)                                                                  \
	class BZDTEST_CLASS_NAME_(testCaseName, testName) : public ::bzd::test::Test                                   \
	{                                                                                                              \
	public:                                                                                                        \
		BZDTEST_CLASS_NAME_(testCaseName, testName)() {}                                                           \
		void test([[maybe_unused]] ::bzd::test::Context& test) const override;                                     \
	};                                                                                                             \
	static auto BZDTEST_REGISTER_NAME_(testCaseName, testName) = ::bzd::test::Manager::getInstance().registerTest( \
		{#testCaseName, #testName, __FILE__, new BZDTEST_CLASS_NAME_(testCaseName, testName)});

#define BZDTEST_(testCaseName, testName)      \
	BZDTEST_REGISTER_(testCaseName, testName) \
	void BZDTEST_CLASS_NAME_(testCaseName, testName)::test([[maybe_unused]] ::bzd::test::Context& test) const

#define BZDTEST_CONSTEXPR_BEGIN_(testCaseName, testName)                                     \
	BZDTEST_REGISTER_(testCaseName, testName)                                                \
	constexpr void BZDTEST_FCT_NAME_(testCaseName, testName)(const ::bzd::test::Context&);   \
	constexpr void BZDTEST_COMPILE_TIME_FCT_NAME_(testCaseName, testName)();                 \
	void BZDTEST_CLASS_NAME_(testCaseName, testName)::test(::bzd::test::Context& test) const \
	{                                                                                        \
		BZDTEST_COMPILE_TIME_FCT_NAME_(testCaseName, testName)();                            \
		BZDTEST_FCT_NAME_(testCaseName, testName)(test);                                     \
	}                                                                                        \
	constexpr void BZDTEST_FCT_NAME_(testCaseName, testName)([[maybe_unused]] const ::bzd::test::Context& test)

#define BZDTEST_CONSTEXPR_END_(testCaseName, testName)                               \
	constexpr void BZDTEST_COMPILE_TIME_FCT_NAME_(testCaseName, testName)()          \
	{                                                                                \
		constexpr auto constexprFct = []() -> bool {                                 \
			constexpr ::bzd::test::Context context{};                                \
			BZDTEST_FCT_NAME_(testCaseName, testName)(context);                      \
			return true;                                                             \
		};                                                                           \
		static_assert(constexprFct(), "Compile time expression evaluation failed."); \
	}

#define BZDTEST_FAIL_FATAL_(...) return ::bzd::test::Manager::getInstance().fail(__FILE__, __LINE__, __VA_ARGS__)
#define BZDTEST_FAIL_NONFATAL_(...) ::bzd::test::Manager::getInstance().fail(__FILE__, __LINE__, __VA_ARGS__)

#define BZDTEST_TEST_BOOLEAN_(condition, actual, expected, failFct)                                                         \
	if (!static_cast<bool>(condition))                                                                                      \
	{                                                                                                                       \
		failFct("Failure\nTest [bool]: " #actual " == " #expected, static_cast<bool>(actual), static_cast<bool>(expected)); \
	}

#define BZDTEST_TEST_EQ_(expression1, expression2, failFct)                                    \
	if (!((expression1) == (expression2)))                                                     \
	{                                                                                          \
		failFct("Failure\nTest: " #expression1 " == " #expression2, expression1, expression2); \
	}

#define BZDTEST_TEST_NE_(expression1, expression2, failFct)                                    \
	if (!((expression1) != (expression2)))                                                     \
	{                                                                                          \
		failFct("Failure\nTest: " #expression1 " != " #expression2, expression1, expression2); \
	}

#define BZDTEST_TEST_LT_(expression1, expression2, failFct)                                   \
	if (!((expression1) < (expression2)))                                                     \
	{                                                                                         \
		failFct("Failure\nTest: " #expression1 " < " #expression2, expression1, expression2); \
	}

#define BZDTEST_TEST_LE_(expression1, expression2, failFct)                                    \
	if (!((expression1) <= (expression2)))                                                     \
	{                                                                                          \
		failFct("Failure\nTest: " #expression1 " <= " #expression2, expression1, expression2); \
	}

#define BZDTEST_TEST_GT_(expression1, expression2, failFct)                                   \
	if (!((expression1) > (expression2)))                                                     \
	{                                                                                         \
		failFct("Failure\nTest: " #expression1 " > " #expression2, expression1, expression2); \
	}

#define BZDTEST_TEST_GE_(expression1, expression2, failFct)                                    \
	if (!((expression1) >= (expression2)))                                                     \
	{                                                                                          \
		failFct("Failure\nTest: " #expression1 " >= " #expression2, expression1, expression2); \
	}

#define BZDTEST_TEST_NEAR_(number1, number2, absError, failFct)                                        \
	if (!bzd::test::impl::near(number1, number2, absError))                                            \
	{                                                                                                  \
		failFct("Failure\nTest: " #number1 " ~== " #number2 " (+/- " #absError ")", number1, number2); \
	}

#define BZDTEST_TEST_STREQ_(str1, str2, failFct)                            \
	if (!bzd::test::impl::strcmp(str1, str2))                               \
	{                                                                       \
		failFct("Failure\nTest [string]: " #str1 " == " #str2, str1, str2); \
	}

#define BZDTEST_TEST_ANY_THROW_(expression, failFct)           \
	{                                                          \
		bool bzdTestIsThrow_ = false;                          \
		try                                                    \
		{                                                      \
			expression;                                        \
		}                                                      \
		catch (...)                                            \
		{                                                      \
			bzdTestIsThrow_ = true;                            \
		}                                                      \
		if (!bzdTestIsThrow_)                                  \
		{                                                      \
			failFct("Failure\nTest: must throw " #expression); \
		}                                                      \
	}

namespace bzd::test::impl {
bool strcmp(const char* str1, const char* str2);
constexpr bool near(const double number1, const double number2, const double absError)
{
	const double diff = (number1 > number2) ? (number1 - number2) : (number2 - number1);
	return (diff <= absError);
}

template <class T>
class Value
{
public:
	constexpr Value(const T& value) { valueToString(buffer_, value); }

	template <class U>
	constexpr Value(U&& value)
	{
		valueToString(buffer_, value);
	}

	constexpr const char* valueToString() const { return buffer_; }

private:
	constexpr char charToString(const char c) { return (c >= 32 && c < 127) ? c : '?'; }

	char* valueToString(char* pBuffer, short value) { return valueToString(pBuffer, static_cast<long long int>(value)); }
	char* valueToString(char* pBuffer, int value) { return valueToString(pBuffer, static_cast<long long int>(value)); }
	char* valueToString(char* pBuffer, long int value) { return valueToString(pBuffer, static_cast<long long int>(value)); }
	char* valueToString(char* pBuffer, unsigned short value) { return valueToString(pBuffer, static_cast<long long int>(value)); }
	char* valueToString(char* pBuffer, unsigned int value) { return valueToString(pBuffer, static_cast<long long int>(value)); }
	char* valueToString(char* pBuffer, unsigned long int value) { return valueToString(pBuffer, static_cast<long long int>(value)); }
	char* valueToString(char* pBuffer, unsigned long long int value) { return valueToString(pBuffer, static_cast<long long int>(value)); }
	char* valueToString(char* pBuffer, long long int value, const int base = 10)
	{
		constexpr char digitToChar[] = "0123456789abcdef";
		const bool isNegative = (value < 0);
		char* ptr = pBuffer + 17;
		value = (isNegative) ? -value : value;

		do
		{
			*--ptr = digitToChar[static_cast<int>(value % base)];
			value = static_cast<long long int>(value / base);
		} while (value);

		if (isNegative)
		{
			*--ptr = '-';
		}

		// Move toward the begining and set the end character
		const int diff = ptr - pBuffer;
		while (*ptr)
		{
			ptr[-diff] = ptr[0];
			++ptr;
		}
		ptr[-diff] = 0;

		return &ptr[-diff];
	}

	char* valueToString(char* pBuffer, float value) { return valueToString(pBuffer, static_cast<double>(value)); }
	char* valueToString(char* pBuffer, double value)
	{
		const long long int valueInteger = static_cast<long long int>(value);
		pBuffer = valueToString(pBuffer, valueInteger);
		*pBuffer++ = '.';

		value -= valueInteger;
		for (int i = 0; i < 10; ++i)
		{
			value *= 10;
			*pBuffer++ = static_cast<char>(static_cast<int>(value) % 10) + '0';
		}

		*pBuffer++ = '\0';
		return pBuffer;
	}

	char* valueToString(char* pBuffer, unsigned char value) { return valueToString(pBuffer, static_cast<char>(value)); }
	char* valueToString(char* pBuffer, char value)
	{
		*pBuffer++ = charToString(value);
		*pBuffer++ = ' ';
		*pBuffer++ = '(';
		*pBuffer++ = '0';
		*pBuffer++ = 'x';
		pBuffer = valueToString(pBuffer, static_cast<unsigned int>(value) & 0xff, 16);
		*pBuffer++ = ')';
		*pBuffer++ = '\0';
		return pBuffer;
	}

	char* valueToString(char* pBuffer, bool value)
	{
		if (value)
		{
			pBuffer[0] = 't';
			pBuffer[1] = 'r';
			pBuffer[2] = 'u';
			pBuffer[3] = 'e';
			pBuffer[4] = '\0';
			return &pBuffer[4];
		}
		pBuffer[0] = 'f';
		pBuffer[1] = 'a';
		pBuffer[2] = 'l';
		pBuffer[3] = 's';
		pBuffer[4] = 'e';
		pBuffer[5] = '\0';
		return &pBuffer[5];
	}

	template <class U>
	char* valueToString(char* pBuffer, U* value)
	{
		*pBuffer++ = '0';
		*pBuffer++ = 'x';
		return valueToString(pBuffer, reinterpret_cast<long long int>(value), 16);
	}

	char* valueToString(char* pBuffer, const unsigned char* value) { return valueToString(pBuffer, reinterpret_cast<const char*>(value)); }
	char* valueToString(char* pBuffer, unsigned char* value) { return valueToString(pBuffer, reinterpret_cast<const char*>(value)); }
	char* valueToString(char* pBuffer, char* value) { return valueToString(pBuffer, static_cast<const char*>(value)); }
	char* valueToString(char* pBuffer, const char* value)
	{
		*pBuffer++ = '"';
		for (int maxChar = 0; maxChar < 64 && *value; ++maxChar)
		{
			const char c = *value++;
			*pBuffer++ = charToString(c);
		}
		if (*value)
		{
			*pBuffer++ = '[';
			*pBuffer++ = '.';
			*pBuffer++ = '.';
			*pBuffer++ = '.';
			*pBuffer++ = ']';
		}
		*pBuffer++ = '"';
		return pBuffer;
	}

	template <class U>
	char* valueToString(char* pBuffer, U&&)
	{
		return pBuffer;
	}

private:
	char buffer_[100]{};
};

} // namespace bzd::test::impl

namespace bzd::test {
class Context
{
public:
	constexpr Context() : seed_{53267} {}

	template <class T>
	[[nodiscard]] T randInt(const T min, const T max) const noexcept
	{
		static ::std::mt19937 gen{seed_};
		::std::uniform_int_distribution<T> distribution(min, max);
		return distribution(gen);
	}

	[[nodiscard]] bool randBool() const noexcept { return (randInt(0, 1) == 1); }

	[[nodiscard]] auto getSeed() const noexcept { return seed_; }

private:
	unsigned int seed_;
};

class Test
{
public:
	Test() = default;
	virtual ~Test() {}
	virtual void test(Context& test) const = 0;
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

	template <class Value1, class Value2>
	void fail(const char* const file, const int line, const char* const message, Value1&& value1, Value2&& value2)
	{
		bzd::test::impl::Value<Value1> valueStr1{value1};
		bzd::test::impl::Value<Value2> valueStr2{value2};
		failInternals(file, line, message, valueStr1.valueToString(), valueStr2.valueToString());
	}

	template <class Value>
	void fail(const char* const file, const int line, const char* const message, Value&& value)
	{
		bzd::test::impl::Value<Value> valueStr{value};
		failInternals(file, line, message, valueStr.valueToString());
	}

	void fail(const char* const file, const int line, const char* const message) { failInternals(file, line, message); }

private:
	void failInternals(
		const char* const file, const int line, const char* const message, const char* actual = nullptr, const char* expected = nullptr);

private:
	Manager() = default;
	bool currentTestFailed_ = false;
};

bzd::Async<bool> run();

} // namespace bzd::test
