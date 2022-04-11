#pragma once

#include "cc/bzd/core/async.hh"
#include "cc/bzd/utility/format/integral.hh"
#include "cc/bzd/utility/numeric_limits.hh"
#include "cc/bzd/utility/random/uniform_int_distribution.hh"
#include "cc/bzd/utility/random/xorwow_engine.hh"

#define BZDTEST_FCT_NAME_(testCaseName, testName) functionBzdTest_##testCaseName##_##testName
#define BZDTEST_CLASS_NAME_(testCaseName, testName) BzdTest_##testCaseName##_##testName
#define BZDTEST_REGISTER_NAME_(testCaseName, testName) registerBzdTest_##testCaseName##_##testName##_
#define BZDTEST_COMPILE_TIME_FCT_NAME_(testCaseName, testName) compileTimeFunctionBzdTest_##testCaseName##_##testName

#define BZDTEST_REGISTER_(testCaseName, testName)                                      \
	class BZDTEST_CLASS_NAME_(testCaseName, testName)                                  \
	{                                                                                  \
	};                                                                                 \
	namespace bzd {                                                                    \
	template <>                                                                        \
	class Test<BZDTEST_CLASS_NAME_(testCaseName, testName)> : public ::bzd::test::Test \
	{                                                                                  \
	public:                                                                            \
		Test() : ::bzd::test::Test{#testCaseName, #testName, __FILE__} {}              \
		void test([[maybe_unused]] ::bzd::test::Context& test) const override;         \
	};                                                                                 \
	}                                                                                  \
	static ::bzd::Test<BZDTEST_CLASS_NAME_(testCaseName, testName)> BZDTEST_REGISTER_NAME_(testCaseName, testName){};

#define BZDTEST_(testCaseName, testName)      \
	BZDTEST_REGISTER_(testCaseName, testName) \
	void ::bzd::Test<BZDTEST_CLASS_NAME_(testCaseName, testName)>::test([[maybe_unused]] ::bzd::test::Context& test) const

#define BZDTEST_ASYNC_(testCaseName, testName)                                                            \
	BZDTEST_REGISTER_(testCaseName, testName)                                                             \
	bzd::Async<> BZDTEST_FCT_NAME_(testCaseName, testName)(const ::bzd::test::Context&);                  \
	void ::bzd::Test<BZDTEST_CLASS_NAME_(testCaseName, testName)>::test(::bzd::test::Context& test) const \
	{                                                                                                     \
		BZDTEST_FCT_NAME_(testCaseName, testName)(test).sync();                                           \
	}                                                                                                     \
	bzd::Async<> BZDTEST_FCT_NAME_(testCaseName, testName)([[maybe_unused]] const ::bzd::test::Context& test)

#define BZDTEST_CONSTEXPR_BEGIN_(testCaseName, testName)                                                  \
	BZDTEST_REGISTER_(testCaseName, testName)                                                             \
	constexpr void BZDTEST_FCT_NAME_(testCaseName, testName)(const ::bzd::test::Context&);                \
	constexpr void BZDTEST_COMPILE_TIME_FCT_NAME_(testCaseName, testName)();                              \
	void ::bzd::Test<BZDTEST_CLASS_NAME_(testCaseName, testName)>::test(::bzd::test::Context& test) const \
	{                                                                                                     \
		BZDTEST_COMPILE_TIME_FCT_NAME_(testCaseName, testName)();                                         \
		BZDTEST_FCT_NAME_(testCaseName, testName)(test);                                                  \
	}                                                                                                     \
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

#define BZDTEST_TEST_ASYNC_BOOLEAN_(result, failFct)                                      \
	if (!static_cast<bool>(result))                                                       \
	{                                                                                     \
		failFct("Failure\nTest [async]: " #result, (result).error().getMessage().data()); \
	}

#define BZDTEST_TEST_EQ_(expression1, expression2, failFct)                    \
	[](const auto& a, const auto& b) {                                         \
		if (!(a == b))                                                         \
		{                                                                      \
			failFct("Failure\nTest: " #expression1 " == " #expression2, a, b); \
		}                                                                      \
	}((expression1), (expression2));

#define BZDTEST_TEST_NE_(expression1, expression2, failFct)                    \
	[](const auto& a, const auto& b) {                                         \
		if (!(a != b))                                                         \
		{                                                                      \
			failFct("Failure\nTest: " #expression1 " != " #expression2, a, b); \
		}                                                                      \
	}((expression1), (expression2));

#define BZDTEST_TEST_LT_(expression1, expression2, failFct)                   \
	[](const auto& a, const auto& b) {                                        \
		if (!(a < b))                                                         \
		{                                                                     \
			failFct("Failure\nTest: " #expression1 " < " #expression2, a, b); \
		}                                                                     \
	}((expression1), (expression2));

#define BZDTEST_TEST_LE_(expression1, expression2, failFct)                    \
	[](const auto& a, const auto& b) {                                         \
		if (!(a <= b))                                                         \
		{                                                                      \
			failFct("Failure\nTest: " #expression1 " <= " #expression2, a, b); \
		}                                                                      \
	}((expression1), (expression2));

#define BZDTEST_TEST_GT_(expression1, expression2, failFct)                   \
	[](const auto& a, const auto& b) {                                        \
		if (!(a > b))                                                         \
		{                                                                     \
			failFct("Failure\nTest: " #expression1 " > " #expression2, a, b); \
		}                                                                     \
	}((expression1), (expression2));

#define BZDTEST_TEST_GE_(expression1, expression2, failFct)                    \
	[](const auto& a, const auto& b) {                                         \
		if (!(a >= b))                                                         \
		{                                                                      \
			failFct("Failure\nTest: " #expression1 " >= " #expression2, a, b); \
		}                                                                      \
	}((expression1), (expression2));

#define BZDTEST_TEST_NEAR_(number1, number2, absError, failFct)                                \
	[](const auto& a, const auto& b, const auto& err) {                                        \
		if (!bzd::test::impl::near(a, b, err))                                                 \
		{                                                                                      \
			failFct("Failure\nTest: " #number1 " ~== " #number2 " (+/- " #absError ")", a, b); \
		}                                                                                      \
	}((number1), (number2), (absError));

#define BZDTEST_TEST_STREQ_(str1, str2, failFct)                          \
	[](const auto& a, const auto& b) {                                    \
		if (bzd::test::impl::strcmp(a, b) != 0)                           \
		{                                                                 \
			failFct("Failure\nTest [string]: " #str1 " == " #str2, a, b); \
		}                                                                 \
	}((str1), (str2));

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

#define BZDTEST_TEST_EQ_VALUES_(container1, container2, failFct)                                            \
	[](const auto& a, const auto& b) {                                                                      \
		if (!(a.size() == b.size()))                                                                        \
		{                                                                                                   \
			failFct("Failure\nTest: " #container1 ".size() == " #container2 ".size()", a.size(), b.size()); \
		}                                                                                                   \
		else                                                                                                \
		{                                                                                                   \
			auto it = b.begin();                                                                            \
			for (const auto& value : a)                                                                     \
			{                                                                                               \
				if (!(value == *it))                                                                        \
				{                                                                                           \
					failFct("Failure\nTest: " #container1 " == " #container2 " at index ", value, *it);     \
				}                                                                                           \
				++it;                                                                                       \
			}                                                                                               \
		}                                                                                                   \
	}((container1), (container2));

namespace bzd::test::impl {
constexpr int strcmp(const char* it1, const char* it2) noexcept
{
	while (*it1 && *it2 && *it1 == *it2)
	{
		++it1;
		++it2;
	}
	if (!(*it1 || *it2))
	{
		return 0;
	}
	return (*it1 < *it2) ? -1 : 1;
}
constexpr bool near(const double number1, const double number2, const double absError) noexcept
{
	const double diff = (number1 > number2) ? (number1 - number2) : (number2 - number1);
	return (diff <= absError);
}

template <class T>
class Value
{
public:
	constexpr Value(const T& value) { valueToString(value); }

	template <class U>
	constexpr Value(U&& value) // NOLINT(bugprone-forwarding-reference-overload)
	{
		valueToString(value);
	}

	constexpr const char* valueToString() const { return string_.data(); }

private:
	constexpr char charToString(const char c) { return (c >= 32 && c < 127) ? c : '?'; }

	constexpr void valueToString(short value) { ::toString(string_, static_cast<bzd::Int64Type>(value)); }
	constexpr void valueToString(int value) { ::toString(string_, static_cast<bzd::Int64Type>(value)); }
	constexpr void valueToString(long int value) { ::toString(string_, static_cast<bzd::Int64Type>(value)); }
	constexpr void valueToString(long long int value) { ::toString(string_, static_cast<bzd::Int64Type>(value)); }
	constexpr void valueToString(unsigned short value) { ::toString(string_, static_cast<bzd::UInt64Type>(value)); }
	constexpr void valueToString(unsigned int value) { ::toString(string_, static_cast<bzd::UInt64Type>(value)); }
	constexpr void valueToString(unsigned long int value) { ::toString(string_, static_cast<bzd::UInt64Type>(value)); }
	constexpr void valueToString(unsigned long long int value) { ::toString(string_, static_cast<bzd::UInt64Type>(value)); }

	constexpr void valueToString(float value) { ::toString(string_, value); }
	constexpr void valueToString(double value) { ::toString(string_, value); }

	constexpr void valueToString(unsigned char value) { valueToString(static_cast<char>(value)); }
	constexpr void valueToString(char value) { ::toString(string_, "{} ({:#x})"_csv, charToString(value), value); }

	constexpr void valueToString(bool value) { string_ = (value) ? "true"_sv : "false"_sv; }

	template <class U>
	constexpr void valueToString(U* value)
	{
		::toString(string_, "{:#x}"_csv, reinterpret_cast<bzd::UInt64Type>(value));
	}

	constexpr void valueToString(const unsigned char* value) { valueToString(reinterpret_cast<const char*>(value)); }
	constexpr void valueToString(unsigned char* value) { valueToString(reinterpret_cast<const char*>(value)); }
	constexpr void valueToString(char* value) { valueToString(static_cast<const char*>(value)); }
	constexpr void valueToString(const char* value)
	{
		string_ += '"';
		for (int maxChar = 0; maxChar < 64 && *value; ++maxChar)
		{
			const char c = *value++;
			string_ += charToString(c);
		}
		if (*value)
		{
			string_ += "[...]"_sv;
		}
		string_ += '"';
	}

	template <class U>
	constexpr void valueToString(U&&)
	{
	}

private:
	bzd::String<100> string_{};
};

} // namespace bzd::test::impl

namespace bzd {
/// Template class for any specialization.
template <class>
class Test
{
};
} // namespace bzd

namespace bzd::test {
class Context
{
public:
	using SeedType = bzd::UInt32Type;

public:
	constexpr explicit Context(const SeedType seed = 53267) noexcept : generator_{seed} {}

	template <class T,
			  T min = NumericLimits<T>::min(),
			  T max = NumericLimits<T>::max(),
			  typename typeTraits::EnableIf<typeTraits::isIntegral<T> && !typeTraits::isSame<T, BoolType>, void>* = nullptr>
	[[nodiscard]] T random() const noexcept
	{
		UniformIntDistribution<T, min, max> distribution{};
		return distribution(generator_);
	}

	template <class T, typename typeTraits::EnableIf<typeTraits::isSame<T, BoolType>, void>* = nullptr>
	[[nodiscard]] T random() const noexcept
	{
		return static_cast<BoolType>(random<bzd::UInt32Type, 0, 1>() == 1);
	}

	template <class T>
	void fillRandom(T& container) const noexcept
	{
		for (auto& value : container)
		{
			value = random<typename T::ValueType>();
		}
	}

private:
	mutable bzd::XorwowEngine generator_;
};

class Test
{
public:
	Test(const char* testCaseName, const char* testName, const char* file);
	virtual ~Test() {}
	virtual void test(Context& test) const = 0;

private:
	/// Generates a deterministic seed from the test identifier.
	constexpr auto getSeed() const noexcept
	{
		Context::SeedType seed{362437};

		for (auto* it : {testCaseName_, testName_, file_})
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

	constexpr bool operator<(const Test& other) const noexcept
	{
		auto result = bzd::test::impl::strcmp(testCaseName_, other.testCaseName_);
		if (result == 0)
		{
			result = bzd::test::impl::strcmp(testName_, other.testName_);
		}
		return (result == -1);
	}

private:
	friend class Manager;

	const char* testCaseName_;
	const char* testName_;
	const char* file_;
	Test* next_{nullptr};
};

class Manager
{
public:
	static Manager& getInstance()
	{
		static Manager instance;
		return instance;
	}
	Manager(Manager const&) = delete;
	void operator=(Manager const&) = delete;

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
