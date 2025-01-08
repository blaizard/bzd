#pragma once

#include "cc/bzd/container/ring_buffer.hh"
#include "cc/bzd/core/async.hh"
#include "cc/bzd/meta/macro.hh"
#include "cc/bzd/test/runner.hh"
#include "cc/bzd/type_traits/is_same_class.hh"
#include "cc/bzd/type_traits/remove_cvref.hh"
#include "cc/bzd/utility/numeric_limits.hh"
#include "cc/bzd/utility/pattern/to_string/integral.hh"
#include "cc/bzd/utility/random/uniform_int_distribution.hh"
#include "cc/bzd/utility/random/xorwow_engine.hh"
#include "cc/bzd/utility/ranges/begin.hh"
#include "cc/bzd/utility/ranges/end.hh"
#include "interfaces/timer.hh"

#define BZDTEST_FCT_NAME_(testCaseName, testName) functionBzdTest_##testCaseName##_##testName
#define BZDTEST_CLASS_NAME_(testCaseName, testName) BZDTEST_CLASS_NAME_2(testCaseName, testName)
#define BZDTEST_CLASS_NAME_2(testCaseName, testName) BzdTest_##testCaseName##_##testName
#define BZDTEST_REGISTER_NAME_(testCaseName, testName) registerBzdTest_##testCaseName##_##testName##_
#define BZDTEST_COMPILE_TIME_FCT_NAME_(testCaseName, testName) compileTimeFunctionBzdTest_##testCaseName##_##testName

#define BZDTEST_REGISTER_(testCaseName, testName, returnType)                                                                              \
	class BZDTEST_CLASS_NAME_(testCaseName, testName) : public ::bzd::test::Test<1>                                                        \
	{                                                                                                                                      \
	public:                                                                                                                                \
		BZDTEST_CLASS_NAME_(testCaseName, testName)() : ::bzd::test::Test<1>{#testCaseName, #testName, __FILE__}                           \
		{                                                                                                                                  \
			this->registerTest(::bzd::test::FunctionPointer::toMember<BZDTEST_CLASS_NAME_(testCaseName, testName),                         \
																	  &BZDTEST_CLASS_NAME_(testCaseName, testName)::test>(*this));         \
		}                                                                                                                                  \
		returnType test([[maybe_unused]] auto& test) const;                                                                                \
	};                                                                                                                                     \
	static BZDTEST_CLASS_NAME_(testCaseName, testName) BZDTEST_REGISTER_NAME_(testCaseName, testName){};

#define BZDTEST_TEMPLATE_REGISTER_(testCaseName, testName, typeList, returnType)                                                           \
	template <class... Types>                                                                                                              \
	class BZDTEST_CLASS_NAME_(testCaseName, testName) : public ::bzd::test::Test<sizeof...(Types)>                                         \
	{                                                                                                                                      \
	public:                                                                                                                                \
		BZDTEST_CLASS_NAME_(testCaseName, testName)() : ::bzd::test::Test<sizeof...(Types)>{#testCaseName, #testName, __FILE__}            \
		{                                                                                                                                  \
			(this->registerTest(::bzd::test::FunctionPointer::toMember<BZDTEST_CLASS_NAME_(testCaseName, testName),                        \
																	   &BZDTEST_CLASS_NAME_(testCaseName, testName)::test<Types>>(*this),  \
								::bzd::test::typeToString<Types>()),                                                                       \
			 ...);                                                                                                                         \
		}                                                                                                                                  \
		template <class TestType>                                                                                                          \
		returnType test([[maybe_unused]] auto& test) const;                                                                                \
	};                                                                                                                                     \
	static BZDTEST_CLASS_NAME_(testCaseName, testName)<BZD_REMOVE_PARENTHESIS(typeList)> BZDTEST_REGISTER_NAME_(testCaseName, testName){};

#define BZDTEST_2(testCaseName, testName)                                                                                                  \
	BZDTEST_REGISTER_(testCaseName, testName, void)                                                                                        \
	void BZDTEST_CLASS_NAME_(testCaseName, testName)::test([[maybe_unused]] auto& test) const

#define BZDTEST_3(testCaseName, testName, typeList)                                                                                        \
	BZDTEST_TEMPLATE_REGISTER_(testCaseName, testName, typeList, void)                                                                     \
	template <class... Types>                                                                                                              \
	template <class TestType>                                                                                                              \
	void BZDTEST_CLASS_NAME_(testCaseName, testName)<Types...>::test([[maybe_unused]] auto& test) const

#define BZDTEST_ASYNC_2(testCaseName, testName)                                                                                            \
	BZDTEST_REGISTER_(testCaseName, testName, ::bzd::Async<>)                                                                              \
	::bzd::Async<> BZDTEST_FCT_NAME_(testCaseName, testName)(auto&);                                                                       \
	::bzd::Async<> BZDTEST_CLASS_NAME_(testCaseName, testName)::test(auto& test) const                                                     \
	{                                                                                                                                      \
		const auto result = co_await BZDTEST_FCT_NAME_(testCaseName, testName)(test);                                                      \
		if (!static_cast<bool>(result))                                                                                                    \
		{                                                                                                                                  \
			BZDTEST_FAIL_MESSAGE_("Failure\nUnhandled failure from async.", result.error().getMessage().data());                           \
		}                                                                                                                                  \
		co_return {};                                                                                                                      \
	}                                                                                                                                      \
	::bzd::Async<> BZDTEST_FCT_NAME_(testCaseName, testName)([[maybe_unused]] auto& test)

#define BZDTEST_ASYNC_3(testCaseName, testName, typeList)                                                                                  \
	BZDTEST_TEMPLATE_REGISTER_(testCaseName, testName, typeList, ::bzd::Async<>)                                                           \
	template <class TestType>                                                                                                              \
	::bzd::Async<> BZDTEST_FCT_NAME_(testCaseName, testName)(auto&);                                                                       \
	template <class... Types>                                                                                                              \
	template <class TestType>                                                                                                              \
	::bzd::Async<> BZDTEST_CLASS_NAME_(testCaseName, testName)<Types...>::test(auto& test) const                                           \
	{                                                                                                                                      \
		const auto result = co_await BZDTEST_FCT_NAME_(testCaseName, testName)<TestType>(test);                                            \
		if (!static_cast<bool>(result))                                                                                                    \
		{                                                                                                                                  \
			BZDTEST_FAIL_MESSAGE_("Failure\nUnhandled failure from async.", result.error().getMessage().data());                           \
		}                                                                                                                                  \
		co_return {};                                                                                                                      \
	}                                                                                                                                      \
	template <class TestType>                                                                                                              \
	::bzd::Async<> BZDTEST_FCT_NAME_(testCaseName, testName)([[maybe_unused]] auto& test)

#define BZDTEST_ASYNC_MULTITHREAD_(testCaseName, testName, nbThreads)                                                                      \
	BZDTEST_REGISTER_(testCaseName, testName, void)                                                                                        \
	::bzd::Async<> BZDTEST_FCT_NAME_(testCaseName, testName)(auto&);                                                                       \
	void BZDTEST_CLASS_NAME_(testCaseName, testName)::test(auto& test) const                                                               \
	{                                                                                                                                      \
		::bzd::async::Executor executor{};                                                                                                 \
		::bzd::Array<std::thread, nbThreads> threads;                                                                                      \
		::bzd::Atomic<::bzd::Bool> isTerminated{false};                                                                                    \
		auto onTerminate = [&isTerminated]() -> ::bzd::Optional<::bzd::async::Executable&> {                                               \
			isTerminated.store(true);                                                                                                      \
			return bzd::nullopt;                                                                                                           \
		};                                                                                                                                 \
		auto promise = BZDTEST_FCT_NAME_(testCaseName, testName)(test);                                                                    \
		promise.enqueue(executor, ::bzd::async::Type::workload, onTerminate);                                                              \
		for (auto& entry : threads)                                                                                                        \
		{                                                                                                                                  \
			entry = ::std::thread{[&executor, &isTerminated]() {                                                                           \
				while (!isTerminated.load())                                                                                               \
				{                                                                                                                          \
					executor.run();                                                                                                        \
				}                                                                                                                          \
			}};                                                                                                                            \
		}                                                                                                                                  \
		for (auto& entry : threads)                                                                                                        \
		{                                                                                                                                  \
			entry.join();                                                                                                                  \
		}                                                                                                                                  \
		const auto result = promise.moveResultOut();                                                                                       \
		if (!static_cast<bool>(result))                                                                                                    \
		{                                                                                                                                  \
			BZDTEST_FAIL_MESSAGE_("Failure\nUnhandled failure from async.", result.error().getMessage().data());                           \
		}                                                                                                                                  \
		BZDTEST_TEST_EQ_(executor.getQueueCount(), 0U, BZDTEST_FAIL_FATAL_);                                                               \
		BZDTEST_TEST_EQ_(executor.getWorkloadCount(), 0, BZDTEST_FAIL_FATAL_);                                                             \
	}                                                                                                                                      \
	::bzd::Async<> BZDTEST_FCT_NAME_(testCaseName, testName)([[maybe_unused]] auto& test)

#define BZDTEST_CONSTEXPR_BEGIN_(testCaseName, testName)                                                                                   \
	BZDTEST_REGISTER_(testCaseName, testName, void)                                                                                        \
	constexpr void BZDTEST_FCT_NAME_(testCaseName, testName)(auto&);                                                                       \
	constexpr void BZDTEST_COMPILE_TIME_FCT_NAME_(testCaseName, testName)();                                                               \
	void BZDTEST_CLASS_NAME_(testCaseName, testName)::test(auto& test) const                                                               \
	{                                                                                                                                      \
		BZDTEST_COMPILE_TIME_FCT_NAME_(testCaseName, testName)();                                                                          \
		BZDTEST_FCT_NAME_(testCaseName, testName)(test);                                                                                   \
	}                                                                                                                                      \
	constexpr void BZDTEST_FCT_NAME_(testCaseName, testName)([[maybe_unused]] auto& test)

#define BZDTEST_CONSTEXPR_END_(testCaseName, testName)                                                                                     \
	constexpr void BZDTEST_COMPILE_TIME_FCT_NAME_(testCaseName, testName)()                                                                \
	{                                                                                                                                      \
		constexpr auto constexprFct = []() -> bool {                                                                                       \
			constexpr ::bzd::test::Context context{};                                                                                      \
			BZDTEST_FCT_NAME_(testCaseName, testName)(context);                                                                            \
			return true;                                                                                                                   \
		};                                                                                                                                 \
		static_assert(constexprFct(), "Compile time expression evaluation failed.");                                                       \
	}

#define BZDTEST_FAIL_FATAL_ return
#define BZDTEST_FAIL_FATAL_ASYNC_                                                                                                          \
	co_return {}
#define BZDTEST_FAIL_NONFATAL_

#define BZDTEST_FAIL_MESSAGE_(...) ::bzd::test::Manager::getInstance().fail(__FILE__, __LINE__, __VA_ARGS__)
#define BZDTEST_FAIL_(condition, ...)                                                                                                      \
	if ((condition))                                                                                                                       \
	{                                                                                                                                      \
		BZDTEST_FAIL_MESSAGE_(__VA_ARGS__);                                                                                                \
		return false;                                                                                                                      \
	}

#define BZDTEST_TEST_TRUE_BOOLEAN_(actual, failFct)                                                                                        \
	if (![](const bool bzdTestCondition_) {                                                                                                \
			BZDTEST_FAIL_(!bzdTestCondition_, "Failure\nTest [bool]: " #actual " == true", bzdTestCondition_, true);                       \
			return true;                                                                                                                   \
		}(static_cast<bool>(actual)))                                                                                                      \
	{                                                                                                                                      \
		failFct;                                                                                                                           \
	}

#define BZDTEST_TEST_FALSE_BOOLEAN_(actual, failFct)                                                                                       \
	if (![](const bool bzdTestCondition_) {                                                                                                \
			BZDTEST_FAIL_(bzdTestCondition_, "Failure\nTest [bool]: " #actual " == false", bzdTestCondition_, false);                      \
			return true;                                                                                                                   \
		}(static_cast<bool>(actual)))                                                                                                      \
	{                                                                                                                                      \
		failFct;                                                                                                                           \
	}

#define BZDTEST_TEST_ASYNC_BOOLEAN_(result, failFct)                                                                                       \
	if (![](const bool bzdTestResult_) {                                                                                                   \
			BZDTEST_FAIL_(!static_cast<bool>(bzdTestResult_),                                                                              \
						  "Failure\nTest [async]: " #result,                                                                               \
						  bzdTestResult_.error().getMessage().data());                                                                     \
			return true;                                                                                                                   \
		}(static_cast<bool>(result)))                                                                                                      \
	{                                                                                                                                      \
		failFct;                                                                                                                           \
	}

#define BZDTEST_TEST_EQ_(expression1, expression2, failFct)                                                                                \
	if (![](const auto& bzdTestA_, const auto& bzdTestB_) {                                                                                \
			BZDTEST_FAIL_(!(bzdTestA_ == bzdTestB_), "Failure\nTest: " #expression1 " == " #expression2, bzdTestA_, bzdTestB_);            \
			return true;                                                                                                                   \
		}((expression1), (expression2)))                                                                                                   \
	{                                                                                                                                      \
		failFct;                                                                                                                           \
	}

#define BZDTEST_TEST_NE_(expression1, expression2, failFct)                                                                                \
	if (![](const auto& bzdTestA_, const auto& bzdTestB_) {                                                                                \
			BZDTEST_FAIL_(!(bzdTestA_ != bzdTestB_), "Failure\nTest: " #expression1 " != " #expression2, bzdTestA_, bzdTestB_);            \
			return true;                                                                                                                   \
		}((expression1), (expression2)))                                                                                                   \
	{                                                                                                                                      \
		failFct;                                                                                                                           \
	}

#define BZDTEST_TEST_LT_(expression1, expression2, failFct)                                                                                \
	if (![](const auto& bzdTestA_, const auto& bzdTestB_) {                                                                                \
			BZDTEST_FAIL_(!(bzdTestA_ < bzdTestB_), "Failure\nTest: " #expression1 " < " #expression2, bzdTestA_, bzdTestB_);              \
			return true;                                                                                                                   \
		}((expression1), (expression2)))                                                                                                   \
	{                                                                                                                                      \
		failFct;                                                                                                                           \
	}

#define BZDTEST_TEST_LE_(expression1, expression2, failFct)                                                                                \
	if (![](const auto& bzdTestA_, const auto& bzdTestB_) {                                                                                \
			BZDTEST_FAIL_(!(bzdTestA_ <= bzdTestB_), "Failure\nTest: " #expression1 " <= " #expression2, bzdTestA_, bzdTestB_);            \
			return true;                                                                                                                   \
		}((expression1), (expression2)))                                                                                                   \
	{                                                                                                                                      \
		failFct;                                                                                                                           \
	}

#define BZDTEST_TEST_GT_(expression1, expression2, failFct)                                                                                \
	if (![](const auto& bzdTestA_, const auto& bzdTestB_) {                                                                                \
			BZDTEST_FAIL_(!(bzdTestA_ > bzdTestB_), "Failure\nTest: " #expression1 " > " #expression2, bzdTestA_, bzdTestB_);              \
			return true;                                                                                                                   \
		}((expression1), (expression2)))                                                                                                   \
	{                                                                                                                                      \
		failFct;                                                                                                                           \
	}

#define BZDTEST_TEST_GE_(expression1, expression2, failFct)                                                                                \
	if (![](const auto& bzdTestA_, const auto& bzdTestB_) {                                                                                \
			BZDTEST_FAIL_(!(bzdTestA_ >= bzdTestB_), "Failure\nTest: " #expression1 " >= " #expression2, bzdTestA_, bzdTestB_);            \
			return true;                                                                                                                   \
		}((expression1), (expression2)))                                                                                                   \
	{                                                                                                                                      \
		failFct;                                                                                                                           \
	}

#define BZDTEST_TEST_NEAR_(number1, number2, absError, failFct)                                                                            \
	if (![](const auto& bzdTestA_, const auto& bzdTestB_, const auto& bzdTestErr_) {                                                       \
			BZDTEST_FAIL_(!bzd::test::impl::near(bzdTestA_, bzdTestB_, bzdTestErr_),                                                       \
						  "Failure\nTest: " #number1 " ~== " #number2 " (+/- " #absError ")",                                              \
						  bzdTestA_,                                                                                                       \
						  bzdTestB_);                                                                                                      \
			return true;                                                                                                                   \
		}((number1), (number2), (absError)))                                                                                               \
	{                                                                                                                                      \
		failFct;                                                                                                                           \
	}

#define BZDTEST_TEST_STREQ_(str1, str2, failFct)                                                                                           \
	if (![](const auto& bzdTestA_, const auto& bzdTestB_) {                                                                                \
			BZDTEST_FAIL_(bzd::test::impl::strcmp(bzdTestA_, bzdTestB_) != 0,                                                              \
						  "Failure\nTest [string]: " #str1 " == " #str2,                                                                   \
						  bzdTestA_,                                                                                                       \
						  bzdTestB_);                                                                                                      \
			return true;                                                                                                                   \
		}((str1), (str2)))                                                                                                                 \
	{                                                                                                                                      \
		failFct;                                                                                                                           \
	}

#define BZDTEST_TEST_ANY_THROW_(expression, failFct)                                                                                       \
	if (![]() {                                                                                                                            \
			bool bzdTestIsThrow_ = false;                                                                                                  \
			try                                                                                                                            \
			{                                                                                                                              \
				expression;                                                                                                                \
			}                                                                                                                              \
			catch (...)                                                                                                                    \
			{                                                                                                                              \
				bzdTestIsThrow_ = true;                                                                                                    \
			}                                                                                                                              \
			BZDTEST_FAIL_(!bzdTestIsThrow_, "Failure\nTest: must throw " #expression);                                                     \
			return true;                                                                                                                   \
		}())                                                                                                                               \
	{                                                                                                                                      \
		failFct;                                                                                                                           \
	}

#define BZDTEST_BEGIN_(range)                                                                                                              \
	[](auto&& bzdTestRange_) {                                                                                                             \
		if constexpr (::bzd::concepts::syncRange<::bzd::typeTraits::RemoveCVRef<decltype(bzdTestRange_)>>)                                 \
		{                                                                                                                                  \
			return ::bzd::Optional{::bzd::begin(bzdTestRange_)};                                                                           \
		}                                                                                                                                  \
		else                                                                                                                               \
		{                                                                                                                                  \
			return ::bzd::begin(bzdTestRange_).sync();                                                                                     \
		}                                                                                                                                  \
	}(range)

#define BZDTEST_INCREMENT_(it)                                                                                                             \
	([](auto& bzdTestIt_) {                                                                                                                \
		if constexpr (::bzd::concepts::syncIterator<::bzd::typeTraits::RemoveCVRef<decltype(bzdTestIt_)>>)                                 \
		{                                                                                                                                  \
			++bzdTestIt_;                                                                                                                  \
			return true;                                                                                                                   \
		}                                                                                                                                  \
		else                                                                                                                               \
		{                                                                                                                                  \
			return (++bzdTestIt_).sync().hasValue();                                                                                       \
		}                                                                                                                                  \
	})(it)

#define BZDTEST_TEST_EQ_RANGE_(range1, range2, failFct)                                                                                    \
	if (![](auto&& bzdTestA_, auto&& bzdTestB_) {                                                                                          \
			auto maybeFirst1 = BZDTEST_BEGIN_(bzdTestA_);                                                                                  \
			BZDTEST_FAIL_(!maybeFirst1, "Failure\nTest: bzd::begin(" #range1 ") failed");                                                  \
			auto& first1 = maybeFirst1.valueMutable();                                                                                     \
			auto maybeFirst2 = BZDTEST_BEGIN_(bzdTestB_);                                                                                  \
			BZDTEST_FAIL_(!maybeFirst2, "Failure\nTest: bzd::begin(" #range2 ") failed");                                                  \
			auto& first2 = maybeFirst2.valueMutable();                                                                                     \
			auto last1 = bzd::end(bzdTestA_);                                                                                              \
			auto last2 = bzd::end(bzdTestB_);                                                                                              \
			using Value1 = ::bzd::typeTraits::RemoveCVRef<decltype(*first1)>;                                                              \
			using Value2 = ::bzd::typeTraits::RemoveCVRef<decltype(*first2)>;                                                              \
			::bzd::test::impl::Values<Value1> values1;                                                                                     \
			::bzd::test::impl::Values<Value2> values2;                                                                                     \
			for (; first1 != last1 && first2 != last2;)                                                                                    \
			{                                                                                                                              \
				values1.pushBack(*first1);                                                                                                 \
				values2.pushBack(*first2);                                                                                                 \
				BZDTEST_FAIL_(!(*first1 == *first2), "Failure\nTest: " #range1 " != " #range2, values1, values2);                          \
				BZDTEST_FAIL_(!BZDTEST_INCREMENT_(first1), "Failure\nTest: ++it1 failed", values1, values2);                               \
				BZDTEST_FAIL_(!BZDTEST_INCREMENT_(first2), "Failure\nTest: ++it2 failed", values1, values2);                               \
			}                                                                                                                              \
			BZDTEST_FAIL_(first1 != last1, "Failure\nTest: " #range2 " terminated while " #range1 " did not");                             \
			BZDTEST_FAIL_(first2 != last2, "Failure\nTest: " #range1 " terminated while " #range2 " did not");                             \
			return true;                                                                                                                   \
		}((range1), (range2)))                                                                                                             \
	{                                                                                                                                      \
		failFct;                                                                                                                           \
	}

namespace bzd::test::impl {

template <class T>
class Values : public ::bzd::RingBuffer<T, 8u>
{
public:
	using ::bzd::RingBuffer<T, 8u>::RingBuffer;
};

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
	constexpr const char* valueToString() const { return string_.data(); }

private:
	constexpr char charToString(char c) { return (c >= 32 && c < 127) ? c : '?'; }

	constexpr void valueToString(short value) { ::bzd::toString(string_.appender(), static_cast<bzd::Int64>(value)); }
	constexpr void valueToString(int value) { ::bzd::toString(string_.appender(), static_cast<bzd::Int64>(value)); }
	constexpr void valueToString(long int value) { ::bzd::toString(string_.appender(), static_cast<bzd::Int64>(value)); }
	constexpr void valueToString(long long int value) { ::bzd::toString(string_.appender(), static_cast<bzd::Int64>(value)); }
	constexpr void valueToString(unsigned short value) { ::bzd::toString(string_.appender(), static_cast<bzd::UInt64>(value)); }
	constexpr void valueToString(unsigned int value) { ::bzd::toString(string_.appender(), static_cast<bzd::UInt64>(value)); }
	constexpr void valueToString(unsigned long int value) { ::bzd::toString(string_.appender(), static_cast<bzd::UInt64>(value)); }
	constexpr void valueToString(unsigned long long int value) { ::bzd::toString(string_.appender(), static_cast<bzd::UInt64>(value)); }

	constexpr void valueToString(float value) { ::bzd::toString(string_.appender(), value); }
	constexpr void valueToString(double value) { ::bzd::toString(string_.appender(), value); }

	constexpr void valueToString(char value) { valueToString(static_cast<unsigned char>(value)); }
	constexpr void valueToString(unsigned char value) { ::bzd::toString(string_.appender(), "{} ({:#x})"_csv, charToString(value), value); }

	constexpr void valueToString(bool value) { string_ = (value) ? "true"_sv : "false"_sv; }

	template <class U>
	constexpr void valueToString(U* value)
	{
		valueToString(static_cast<const U*>(value));
	}
	template <class U>
	constexpr void valueToString(const U* value)
	{
		::bzd::toString(string_.appender(), "{:#x}"_csv, reinterpret_cast<bzd::UInt64>(value));
	}

	constexpr void valueToString(const unsigned char* value) { valueToString(reinterpret_cast<const char*>(value)); }
	constexpr void valueToString(unsigned char* value) { valueToString(reinterpret_cast<const char*>(value)); }
	constexpr void valueToString(char* value) { valueToString(static_cast<const char*>(value)); }
	constexpr void valueToString(const char* value)
	{
		const char* ptr = value;
		string_ += '"';
		for (int maxChar = 0; maxChar < 64 && *ptr; ++maxChar)
		{
			const char c = *ptr++;
			string_ += charToString(c);
		}
		if (*ptr)
		{
			string_ += "[...]"_sv;
		}
		string_ += '"';
	}
	constexpr void valueToString(::bzd::Byte value)
	{
		::bzd::toString(string_.appender(), "{:#x}({})"_csv, value, charToString(static_cast<char>(value)));
	}

	template <class U>
	constexpr void valueToString(Values<U>& values)
	{
		if (values.overrun())
		{
			string_ += "[...]"_sv;
		}
		for (const auto& value : values.asSpans())
		{
			valueToString(value);
			string_ += " "_sv;
		}
	}

	template <class U>
	void valueToString(U&& values)
	{
		const auto* const data = reinterpret_cast<const ::bzd::Byte*>(&values);

		::bzd::toString(string_.appender(), "[Binary ({} bytes)] "_csv, sizeof(U));
		for (bzd::UInt32 i = 0; i < 8u && i < sizeof(U); ++i)
		{
			// NOLINTNEXTLINE(clang-analyzer-core.CallAndMessage)
			valueToString(data[i]);
			string_ += " "_sv;
		}
		if (8u < sizeof(U))
		{
			string_ += "[...]"_sv;
		}
	}

private:
	::bzd::String<100> string_{};
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

class Runner;

class Context
{
public:
	using SeedType = bzd::UInt32;

public:
	Context() = default;
	constexpr explicit Context(const SeedType seed, bzd::Timer& timer, bzd::test::Runner& runner) noexcept :
		generator_{seed}, timer_{&timer}, runner_{&runner}
	{
	}

	template <class T, T min = NumericLimits<T>::min(), T max = NumericLimits<T>::max()>
	requires(concepts::integral<T> && !concepts::sameClassAs<T, Bool>)
	[[nodiscard]] T random() noexcept
	{
		UniformIntDistribution<T, min, max> distribution{};
		return distribution(generator_);
	}

	template <class T>
	requires(concepts::sameClassAs<T, Bool>)
	[[nodiscard]] T random() noexcept
	{
		return static_cast<Bool>(random<bzd::UInt32, 0, 1>() == 1);
	}

	template <class T>
	void fillRandom(T& container) noexcept
	{
		for (auto& value : container)
		{
			value = random<typename T::ValueType>();
		}
	}

	bzd::Timer& timer() noexcept { return *timer_; }

	template <class T>
	T& getRunner() noexcept
	{
		return static_cast<T&>(*runner_);
	}

private:
	bzd::XorwowEngine generator_{53267};
	bzd::Timer* timer_{nullptr};
	bzd::test::Runner* runner_{nullptr};
};

/// Convert a type into a string view at compile type.
template <class T>
constexpr auto typeToString()
{
#if defined(__clang__)
	constexpr bzd::StringView prefix{"[T = "};
	constexpr bzd::StringView suffix{"]"};
	constexpr bzd::StringView function{__PRETTY_FUNCTION__};
#elif defined(__GNUC__)
	constexpr bzd::StringView prefix{"with T = "};
	constexpr bzd::StringView suffix{"]"};
	constexpr bzd::StringView function{__PRETTY_FUNCTION__};
#else
#error Unsupported compiler
#endif

	constexpr auto start = function.find(prefix) + prefix.size();
	constexpr auto end = function.rfind(suffix);
	static_assert(start < end);

	return function.subStr(start, (end - start));
}

/// Holds information about the test class.
struct TestInfo
{
	const char* testCaseName;
	const char* testName;
	const char* file;

	/// Generates a deterministic seed from the test identifier.
	constexpr auto getSeed() const noexcept
	{
		Context::SeedType seed{362437};

		for (auto* it : {testCaseName, testName, file})
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

struct FunctionPointer
{
	template <class Object, void (Object::*memberPtr)(::bzd::test::Context&) const>
	static constexpr auto toMember(Object& object) noexcept
	{
		FunctionPointer pointer;
		pointer.function_ = wrapper<Object, memberPtr>;
		pointer.object_ = static_cast<void*>(&object);
		return pointer;
	}

	template <class Object, ::bzd::Async<> (Object::*memberPtr)(::bzd::test::Context&) const>
	static constexpr auto toMember(Object& object) noexcept
	{
		FunctionPointer pointer;
		pointer.functionAsync_ = wrapper<Object, memberPtr>;
		pointer.object_ = static_cast<void*>(&object);
		return pointer;
	}

	constexpr bool isAsync() const noexcept { return function_ == nullptr; }

	constexpr void run(::bzd::test::Context& context) { return function_(object_, context); }
	::bzd::Async<> runAsync(::bzd::test::Context& context) { co_return co_await functionAsync_(object_, context); }

private:
	template <class Object, void (Object::*memberPtr)(::bzd::test::Context&) const>
	static void wrapper(const void* objectErased, ::bzd::test::Context& context)
	{
		const auto object = static_cast<const Object*>(objectErased);
		return (object->*memberPtr)(context);
	}

	template <class Object, ::bzd::Async<> (Object::*memberPtr)(::bzd::test::Context&) const>
	static ::bzd::Async<> wrapper(const void* objectErased, ::bzd::test::Context& context)
	{
		const auto object = static_cast<const Object*>(objectErased);
		co_return co_await (object->*memberPtr)(context);
	}

	using Pointer = void (*)(const void*, ::bzd::test::Context&);
	Pointer function_{nullptr};
	using PointerAsync = ::bzd::Async<> (*)(const void*, ::bzd::test::Context&);
	PointerAsync functionAsync_{nullptr};
	void* object_{nullptr};
};

// Hold information about the next test instance to execute.
struct TestNode
{
	/// Tells if a node is valid or not.
	constexpr bool isValid() noexcept { return (info != nullptr); }
	TestInfo* info{nullptr};
	FunctionPointer function{};
	::bzd::StringView variant{};
	TestNode* next{nullptr};
};

template <Size n>
class Test
{
public:
	Test(const char* testCaseName, const char* testName, const char* file) noexcept : info_{testCaseName, testName, file} {}

protected:
	/// Register a specific test instance to be executed.
	///
	/// \param test The test function pointer, the entry point of the test to be executed.
	/// \param variant The variant name to use for this instance.
	void registerTest(FunctionPointer function, ::bzd::StringView variant = ""_sv) noexcept
	{
		extern bzd::test::TestNode* nodeCurrent;
		nodeCurrent->info = &info_;
		nodeCurrent->function = function;
		nodeCurrent->variant = variant;
		nodeCurrent->next = nextNode_++;
		nodeCurrent = nodeCurrent->next;
	}

private:
	friend class Manager;

	TestInfo info_;
	TestNode node_[n];
	TestNode* nextNode_{node_};
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

	bzd::Async<bool> run(bzd::OStream& out, bzd::Timer& timer, bzd::test::Runner& runner);

	template <class Value1, class Value2>
	void fail(const char* const file, const bzd::Int32 line, const char* const message, Value1&& value1, Value2&& value2)
	{
		bzd::test::impl::Value<Value1> valueStr1{value1};
		bzd::test::impl::Value<Value2> valueStr2{value2};
		failInternals(file, line, message, valueStr1.valueToString(), valueStr2.valueToString());
	}

	template <class Value>
	void fail(const char* const file, const bzd::Int32 line, const char* const message, Value&& value)
	{
		bzd::test::impl::Value<Value> valueStr{value};
		failInternals(file, line, message, valueStr.valueToString());
	}

	void fail(const char* const file, const bzd::Int32 line, const char* const message) { failInternals(file, line, message); }

private:
	void failInternals(const char* const file,
					   const bzd::Int32 line,
					   const char* const message,
					   const char* actual = nullptr,
					   const char* expected = nullptr);

private:
	Manager() = default;
	bool currentTestFailed_ = false;
};

class Runner
{
public:
	Runner() = default;
	constexpr Runner(auto&&) noexcept {}

	bzd::Async<bool> run(bzd::OStream& out, bzd::Timer& timer)
	{
		if (co_await !::bzd::test::Manager::getInstance().run(out, timer, *this))
		{
			co_return true;
		}
		co_return ::bzd::error::Failure("Test failed."_csv);
	}
};

} // namespace bzd::test
